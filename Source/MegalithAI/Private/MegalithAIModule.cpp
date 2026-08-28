#include "MegalithAIModule.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithAIBlackboardActions.h"
#include "MegalithAIBehaviorTreeActions.h"
#include "MegalithAIStateTreeActions.h"
#include "MegalithAIEQSActions.h"
#include "MegalithAIControllerActions.h"
#include "MegalithAIPerceptionActions.h"
#include "MegalithAIPerceptionScaffoldActions.h"  // F8: add_perception_to_actor
#include "MegalithAISmartObjectActions.h"
#include "MegalithAINavigationActions.h"
#include "MegalithAIRuntimeActions.h"
#include "MegalithAIScaffoldActions.h"
#include "MegalithAIDiscoveryActions.h"
#include "MegalithAIAdvancedActions.h"
#include "MegalithAIIndexer.h"
#include "MegalithIndexSubsystem.h"
#include "MegalithAIBulkFillAdapter.h"
#include "Editor.h"

DEFINE_LOG_CATEGORY(LogMegalithAI);

void FMegalithAIModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings || !Settings->bEnableAI)
	{
		UE_LOG(LogMegalithAI, Log,
			TEXT("MegalithAI: AI integration disabled in settings"));
		return;
	}

	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithAIBlackboardActions::RegisterActions(Registry);
	FMegalithAIBehaviorTreeActions::RegisterActions(Registry);
	FMegalithAIStateTreeActions::RegisterActions(Registry);
	FMegalithAIEQSActions::RegisterActions(Registry);
	FMegalithAIControllerActions::RegisterActions(Registry);
	FMegalithAIPerceptionActions::RegisterActions(Registry);
	FMegalithAIPerceptionScaffoldActions::RegisterActions(Registry);  // F8: add_perception_to_actor
	FMegalithAISmartObjectActions::RegisterActions(Registry);
	FMegalithAINavigationActions::RegisterActions(Registry);
	FMegalithAIRuntimeActions::RegisterActions(Registry);
	FMegalithAIScaffoldActions::RegisterActions(Registry);
	FMegalithAIDiscoveryActions::RegisterActions(Registry);
	FMegalithAIAdvancedActions::RegisterActions(Registry);

	// Phase 5 Step 1 (MCP Ergonomics, 2026-05-11) — register the ai adapter on the
	// central FMegalithBulkFillRegistry. No WITH_* gate needed (AIModule is always-on
	// engine core). Body delegates EQS-tests/BB-keys/SmartObject-slots fill_kinds to
	// FMegalithReflectionWalker.
	FMegalithAIBulkFillAdapter::Register();

	// Register the AI deep indexer into MegalithIndex (deferred until editor subsystems are ready)
	if (Settings->bIndexAI)
	{
		PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
		{
			if (GEditor)
			{
				if (UMegalithIndexSubsystem* IndexSS = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>())
				{
					IndexSS->RegisterIndexer(MakeShared<FAIIndexer>());
					UE_LOG(LogMegalithAI, Log, TEXT("MegalithAI: Registered FAIIndexer into MegalithIndex"));
				}
			}
		});
	}

	int32 ActionCount = Registry.GetActions(TEXT("ai")).Num();
	const TCHAR* MassStatus =
#if WITH_MASSENTITY
		TEXT("available");
#else
		TEXT("not installed");
#endif
	UE_LOG(LogMegalithAI, Log, TEXT("MegalithAI: Loaded (%d actions, MassEntity=%s)"), ActionCount, MassStatus);
}

void FMegalithAIModule::ShutdownModule()
{
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
		PostEngineInitHandle.Reset();
	}

	FMegalithAIBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("ai"));
}

IMPLEMENT_MODULE(FMegalithAIModule, MegalithAI)
