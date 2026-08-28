#include "MegalithEditorModule.h"
#include "MegalithEditorActions.h"
#include "MegalithEditorMapActions.h"
#include "MegalithPieObjectActions.h"
#include "MegalithPieInputActions.h"
#include "MegalithPieTimeseries.h"
#include "MegalithStatActions.h"
#include "MegalithSettingsCustomization.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithSettings.h"
#include "PropertyEditorModule.h"
#include "Misc/OutputDeviceRedirector.h"

// PART C — passive modal watcher.
#include "Misc/CoreDelegates.h"
#include "Misc/EngineVersionComparison.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/SWindow.h"
#include "Widgets/Text/STextBlock.h"
#include "Layout/Children.h"

#define LOCTEXT_NAMESPACE "FMegalithEditorModule"

namespace
{
	// Recursively walk a Slate widget subtree, appending the text of every STextBlock
	// found to OutText (newline-joined). Best-effort and depth-bounded so a pathological
	// tree can't stall the broadcast. The broadcasting thread is the game thread; this
	// runs before the modal's nested loop starts.
	void HarvestTextBlocks(const TSharedPtr<SWidget>& Widget, FString& OutText, int32 Depth)
	{
		if (!Widget.IsValid() || Depth > 12)
		{
			return;
		}
		// STextBlock is a SLeafWidget — identify by widget type name (no RTTI dependency).
		if (Widget->GetType() == TEXT("STextBlock"))
		{
			const FText Text = StaticCastSharedPtr<STextBlock>(Widget)->GetText();
			if (!Text.IsEmpty())
			{
				if (!OutText.IsEmpty()) { OutText.Append(TEXT(" | ")); }
				OutText.Append(Text.ToString());
			}
		}
		if (FChildren* Children = Widget->GetChildren())
		{
			const int32 Num = Children->Num();
			for (int32 Index = 0; Index < Num; ++Index)
			{
				HarvestTextBlocks(Children->GetChildAt(Index), OutText, Depth + 1);
			}
		}
	}

	// Best-effort title + message harvest of the active (or about-to-be-active) modal
	// window. The window may not yet be on the modal stack at PRE broadcast time, so
	// fall back to the active top-level window.
	void HarvestActiveModalWindow(FString& OutTitle, FString& OutText)
	{
		if (!FSlateApplication::IsInitialized())
		{
			return;
		}
		FSlateApplication& Slate = FSlateApplication::Get();
		TSharedPtr<SWindow> Window = Slate.GetActiveModalWindow();
		if (!Window.IsValid())
		{
			Window = Slate.GetActiveTopLevelWindow();
		}
		if (Window.IsValid())
		{
			OutTitle = Window->GetTitle().ToString();
			HarvestTextBlocks(Window->GetContent(), OutText, 0);
		}
	}
}

void FMegalithEditorModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableEditor) return;

	LogCapture = new FMegalithLogCapture();
	GLog->AddOutputDevice(LogCapture);

	FMegalithEditorActions::RegisterActions(LogCapture);
	FMegalithEditorMapActions::RegisterActions(FMegalithToolRegistry::Get());  // F8: create_empty_map + get_module_status
	// Gap 8: live-PIE object property read + function call (editor namespace).
	FMegalithPieObjectActions::RegisterActions(FMegalithToolRegistry::Get());
	// Gap 9: time-series PIE sampling with scripted provocation. Implemented in
	// MegalithEditor (it owns the async PIE-smoke session machinery) but registered
	// under the "animation" namespace string — the registry is namespace-string-keyed,
	// not module-keyed (see UnregisterNamespace note in ShutdownModule).
	FMegalithPieTimeseries::RegisterActions(FMegalithToolRegistry::Get());
	// Gap 4: deterministic PIE input/control driving (set control rotation with hold,
	// inject Enhanced Input action, free-fly spectator possess). The held-rotation /
	// repeated-input re-apply state is dropped on PIE end via the hook below.
	FMegalithPieInputActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithPieInputActions::RegisterPieEndHook();
	// Gap 10: programmatic stat-group counter/cycle readout (#if STATS gated).
	FMegalithStatActions::RegisterActions(FMegalithToolRegistry::Get());

	// Register settings detail customization
	FPropertyEditorModule& PropModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropModule.RegisterCustomClassLayout(
		UMegalithSettings::StaticClass()->GetFName(),
		FOnGetDetailCustomizationInstance::CreateStatic(&FMegalithSettingsCustomization::MakeInstance)
	);

	const int32 EditorActionCount = FMegalithToolRegistry::Get().GetActions(TEXT("editor")).Num();
	UE_LOG(LogMegalith, Log, TEXT("Megalith — Editor module loaded (%d editor actions)"), EditorActionCount);

	// PART C — subscribe to the paired pre/post Slate-modal broadcasts so an external
	// consumer tailing the log can pair MODAL_OPEN with MODAL_CLOSE: an unmatched OPEN
	// past a grace period means the game thread is still inside the blocking nested
	// loop (and the in-process MCP server is starved).
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8
	PreSlateModalHandle = FCoreDelegates::PreSlateModalWithContext.AddRaw(this, &FMegalithEditorModule::OnPreSlateModal);
	PostSlateModalHandle = FCoreDelegates::PostSlateModalWithContext.AddRaw(this, &FMegalithEditorModule::OnPostSlateModal);
#else
	PreSlateModalHandle = FCoreDelegates::PreSlateModal.AddRaw(this, &FMegalithEditorModule::OnPreSlateModal);
	PostSlateModalHandle = FCoreDelegates::PostSlateModal.AddRaw(this, &FMegalithEditorModule::OnPostSlateModal);
#endif
#endif
}

#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8

void FMegalithEditorModule::OnPreSlateModal(const FCoreDelegates::FModalWindowContext& Context)
{
	// Always emit at least a timestamped record — text extraction is best-effort
	// (the window may not yet be on the modal stack at broadcast time).
	FString Title;
	FString Text;
	HarvestActiveModalWindow(Title, Text);

	FOpenModalRecord Record;
	Record.Title = Title;
	Record.SlowTask = Context.bIsSlowTaskWindow.IsSet()
		? (Context.bIsSlowTaskWindow.GetValue() ? TEXT("true") : TEXT("false"))
		: TEXT("unknown");
	Record.OpenedAt = FDateTime::Now();

	const int64 Id = static_cast<int64>(Context.WindowIdentifier);
	OpenModals.Add(Id, Record);

	UE_LOG(LogMegalith, Warning,
		TEXT("MODAL_OPEN ts='%s' id=%lld slow_task=%s title='%s' text='%s' — game thread is about to enter a blocking modal loop; MCP will be unresponsive until dismissed."),
		*Record.OpenedAt.ToString(TEXT("%Y-%m-%dT%H:%M:%S")), Id, *Record.SlowTask, *Title, *Text);
}

void FMegalithEditorModule::OnPostSlateModal(const FCoreDelegates::FModalWindowContext& Context)
{
	const int64 Id = static_cast<int64>(Context.WindowIdentifier);

	// Echo the cached open-record fields; an unmatched close (subscribed mid-modal,
	// or an open we never saw) still emits a parseable record.
	FString Title;
	FString SlowTask = TEXT("unknown");
	FString OpenAge = TEXT("unknown");
	if (const FOpenModalRecord* Record = OpenModals.Find(Id))
	{
		Title = Record->Title;
		SlowTask = Record->SlowTask;
		OpenAge = FString::Printf(TEXT("%.1f"), (FDateTime::Now() - Record->OpenedAt).GetTotalSeconds());
		OpenModals.Remove(Id);
	}

	UE_LOG(LogMegalith, Warning,
		TEXT("MODAL_CLOSE ts='%s' id=%lld slow_task=%s title='%s' open_age_s=%s — modal dismissed; game thread resumed."),
		*FDateTime::Now().ToString(TEXT("%Y-%m-%dT%H:%M:%S")), Id, *SlowTask, *Title, *OpenAge);
}

#else // pre-5.8: no WithContext delegates — paired records without id/slow-task.

void FMegalithEditorModule::OnPreSlateModal()
{
	FString Title;
	FString Text;
	HarvestActiveModalWindow(Title, Text);

	UE_LOG(LogMegalith, Warning,
		TEXT("MODAL_OPEN ts='%s' id=0 slow_task=unknown title='%s' text='%s' — game thread is about to enter a blocking modal loop; MCP will be unresponsive until dismissed."),
		*FDateTime::Now().ToString(TEXT("%Y-%m-%dT%H:%M:%S")), *Title, *Text);
}

void FMegalithEditorModule::OnPostSlateModal()
{
	UE_LOG(LogMegalith, Warning,
		TEXT("MODAL_CLOSE ts='%s' id=0 slow_task=unknown title='' open_age_s=unknown — modal dismissed; game thread resumed."),
		*FDateTime::Now().ToString(TEXT("%Y-%m-%dT%H:%M:%S")));
}

#endif // engine version
#endif // WITH_EDITOR

void FMegalithEditorModule::ShutdownModule()
{
#if WITH_EDITOR
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 8
	if (PreSlateModalHandle.IsValid())
	{
		FCoreDelegates::PreSlateModalWithContext.Remove(PreSlateModalHandle);
		PreSlateModalHandle.Reset();
	}
	if (PostSlateModalHandle.IsValid())
	{
		FCoreDelegates::PostSlateModalWithContext.Remove(PostSlateModalHandle);
		PostSlateModalHandle.Reset();
	}
	OpenModals.Empty();
#else
	if (PreSlateModalHandle.IsValid())
	{
		FCoreDelegates::PreSlateModal.Remove(PreSlateModalHandle);
		PreSlateModalHandle.Reset();
	}
	if (PostSlateModalHandle.IsValid())
	{
		FCoreDelegates::PostSlateModal.Remove(PostSlateModalHandle);
		PostSlateModalHandle.Reset();
	}
#endif
#endif

	// Gap 4: drop the PIE-end hook + any residual held-rotation / repeated-input / spectator state.
	FMegalithPieInputActions::UnregisterPieEndHook();

	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("editor"));

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropModule.UnregisterCustomClassLayout(UMegalithSettings::StaticClass()->GetFName());
	}

	if (LogCapture)
	{
		GLog->RemoveOutputDevice(LogCapture);
		delete LogCapture;
		LogCapture = nullptr;
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithEditorModule, MegalithEditor)
