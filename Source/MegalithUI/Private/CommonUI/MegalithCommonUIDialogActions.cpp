// Category G: Runtime Dialog / Messaging — 2 actions
// 4.G.1 show_common_message [RUNTIME]
// 4.G.2 configure_modal_overlay
#include "MegalithCommonUIHelpers.h"

#if WITH_COMMONUI

#include "MegalithToolRegistry.h"
#include "MegalithParamSchema.h"
#include "MegalithJsonUtils.h"

#include "CommonActivatableWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "Components/BackgroundBlur.h"
#include "WidgetBlueprint.h"
#include "Blueprint/WidgetTree.h"
#include "Components/PanelWidget.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "UObject/UObjectIterator.h"
#include "Engine/World.h"

namespace MegalithCommonUIDialog
{
	// ----- 4.G.1 show_common_message [RUNTIME] ---------------------------------
	// Stock UE 5.7 has no built-in UCommonGameDialog class (lives in Lyra's CommonGame).
	// This action takes a user-provided dialog WBP class + an activatable container name
	// and pushes the dialog onto the container's stack. Caller is responsible for wiring
	// message text into the dialog via its own BP graph.

	static FMegalithActionResult HandleShowCommonMessage(const TSharedPtr<FJsonObject>& Params)
	{
		if (!MegalithCommonUI::GetPIEWorld())
			return FMegalithActionResult::Error(TEXT("requires PIE session"));

		FString ContainerName, DialogClassPath;
		if (!Params.IsValid() ||
			!Params->TryGetStringField(TEXT("container_name"), ContainerName) ||
			!Params->TryGetStringField(TEXT("dialog_class"), DialogClassPath))
			return FMegalithActionResult::Error(TEXT("container_name and dialog_class required"));

		UWorld* PIE = MegalithCommonUI::GetPIEWorld();
		UCommonActivatableWidgetContainerBase* Container = nullptr;
		for (TObjectIterator<UCommonActivatableWidgetContainerBase> It; It; ++It)
		{
			UCommonActivatableWidgetContainerBase* C = *It;
			if (!C || C->GetWorld() != PIE) continue;
			if (C->GetFName() == FName(*ContainerName)) { Container = C; break; }
		}
		if (!Container)
			return FMegalithActionResult::Error(FString::Printf(TEXT("Container '%s' not found in PIE"), *ContainerName));

		UClass* DialogClass = LoadClass<UCommonActivatableWidget>(nullptr, *DialogClassPath);
		if (!DialogClass)
			return FMegalithActionResult::Error(FString::Printf(TEXT("Failed to resolve dialog class '%s'"), *DialogClassPath));

		UCommonActivatableWidget* Pushed = Container->AddWidget(DialogClass);

		TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("container_name"), ContainerName);
		Result->SetStringField(TEXT("dialog_class"), DialogClass->GetName());
		Result->SetBoolField(TEXT("pushed"), Pushed != nullptr);
		Result->SetNumberField(TEXT("stack_depth"), Container->GetNumWidgets());
		Result->SetStringField(TEXT("note"), TEXT("Dialog pushed. Result-binding (Yes/No/Cancel) must be wired in dialog WBP's BP graph — async delegates not yet routed through MCP."));
		return FMegalithActionResult::Success(Result);
	}

	// ----- 4.G.2 configure_modal_overlay ---------------------------------------
	// Add a UBackgroundBlur widget behind a named parent (intended use: place before an
	// activatable stack's slot so modal widgets dim/blur the scene behind them).

	static FMegalithActionResult HandleConfigureModalOverlay(const TSharedPtr<FJsonObject>& Params)
	{
		FString WbpPath, ParentName, BlurName = TEXT("ModalBackdropBlur");
		double BlurStrength = 8.0;
		if (!Params.IsValid() ||
			!Params->TryGetStringField(TEXT("parent_widget"), ParentName))
			return FMegalithActionResult::Error(TEXT("wbp_path and parent_widget required"));
		WbpPath = MegalithCommonUI::GetWbpPath(Params);
		if (WbpPath.IsEmpty())
			return FMegalithActionResult::Error(TEXT("wbp_path (or asset_path) required"));
		Params->TryGetStringField(TEXT("blur_widget_name"), BlurName);
		Params->TryGetNumberField(TEXT("blur_strength"), BlurStrength);

		UWidgetBlueprint* Wbp = LoadObject<UWidgetBlueprint>(nullptr, *WbpPath);
		if (!Wbp || !Wbp->WidgetTree)
			return FMegalithActionResult::Error(FString::Printf(TEXT("Failed to load WBP '%s'"), *WbpPath));

		UPanelWidget* Parent = nullptr;
		Wbp->WidgetTree->ForEachWidget([&](UWidget* W)
		{
			if (!Parent && W && W->GetFName() == FName(*ParentName))
				Parent = Cast<UPanelWidget>(W);
		});
		if (!Parent)
			return FMegalithActionResult::Error(FString::Printf(TEXT("Parent '%s' not found or not a panel"), *ParentName));

		UBackgroundBlur* Blur = Wbp->WidgetTree->ConstructWidget<UBackgroundBlur>(UBackgroundBlur::StaticClass(), FName(*BlurName));
		Blur->SetBlurStrength(static_cast<float>(BlurStrength));
		Parent->AddChild(Blur);

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Wbp);
		FKismetEditorUtilities::CompileBlueprint(Wbp);
		Wbp->GetOutermost()->MarkPackageDirty();

		TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("wbp_path"), WbpPath);
		Result->SetStringField(TEXT("parent_widget"), ParentName);
		Result->SetStringField(TEXT("blur_widget_name"), BlurName);
		Result->SetNumberField(TEXT("blur_strength"), BlurStrength);
		return FMegalithActionResult::Success(Result);
	}

	// ----- Registration --------------------------------------------------------

	void Register(FMegalithToolRegistry& Registry)
	{
		const FString Cat(TEXT("CommonUI"));

		Registry.RegisterAction(
			TEXT("ui"), TEXT("show_common_message"),
			TEXT("[RUNTIME] Push a dialog WBP class onto a named activatable container. Result-binding (Yes/No) is dialog-WBP-responsible; this is fire-and-forward."),
			FMegalithActionHandler::CreateStatic(&HandleShowCommonMessage),
			FParamSchemaBuilder()
				.Required(TEXT("container_name"), TEXT("string"), TEXT("FName of UCommonActivatableWidgetContainerBase in PIE (modal layer)"))
				.Required(TEXT("dialog_class"), TEXT("string"), TEXT("Dialog WBP class path (subclass of UCommonActivatableWidget)"))
				.Build(),
			Cat);

		Registry.RegisterAction(
			TEXT("ui"), TEXT("configure_modal_overlay"),
			TEXT("Add a UBackgroundBlur widget behind a parent panel in a WBP. Intended for modal dim/blur effect."),
			FMegalithActionHandler::CreateStatic(&HandleConfigureModalOverlay),
			FParamSchemaBuilder()
				.RequiredAssetPath(TEXT("wbp_path"), TEXT("Widget Blueprint path"))
				.Required(TEXT("parent_widget"), TEXT("string"), TEXT("Parent panel name to add blur into"))
				.Optional(TEXT("blur_widget_name"), TEXT("string"), TEXT("Name for the blur widget"), TEXT("ModalBackdropBlur"))
				.Optional(TEXT("blur_strength"), TEXT("number"), TEXT("Blur radius"))
				.Build(),
			Cat);
	}
}

#endif // WITH_COMMONUI
