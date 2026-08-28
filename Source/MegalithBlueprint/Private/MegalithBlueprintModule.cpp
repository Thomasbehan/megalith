#include "MegalithBlueprintModule.h"
#include "MegalithBlueprintActions.h"
#include "MegalithBlueprintVariableActions.h"
#include "MegalithBlueprintContractActions.h"
#include "MegalithBlueprintComponentActions.h"
#include "MegalithBlueprintGraphActions.h"
#include "MegalithBlueprintNodeActions.h"
#include "MegalithBlueprintCompileActions.h"
#include "MegalithBlueprintCDOActions.h"
#include "MegalithBlueprintStructActions.h"
#include "MegalithBlueprintDataTableActions.h"
#include "MegalithBlueprintCurveTableActions.h"
#include "MegalithBlueprintStringTableActions.h"
#include "MegalithBlueprintBuildActions.h"
#include "MegalithBlueprintDiffActions.h"
#include "MegalithBlueprintTemplateActions.h"
#include "MegalithBlueprintGraphExportActions.h"
#include "MegalithBlueprintLayoutActions.h"
#include "MegalithBlueprintSpawnActions.h"
#include "MegalithMotionMatchingScaffoldActions.h"
#include "MegalithBlueprintBulkFillAdapter.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithSettings.h"

#define LOCTEXT_NAMESPACE "FMegalithBlueprintModule"

void FMegalithBlueprintModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableBlueprint) return;

	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithBlueprintActions::RegisterActions();
	FMegalithBlueprintVariableActions::RegisterActions(Registry);
	FMegalithBlueprintContractActions::RegisterActions(Registry);
	FMegalithBlueprintComponentActions::RegisterActions(Registry);
	FMegalithBlueprintGraphActions::RegisterActions(Registry);
	FMegalithBlueprintNodeActions::RegisterActions(Registry);
	FMegalithBlueprintCompileActions::RegisterActions(Registry);
	FMegalithBlueprintCDOActions::RegisterActions(Registry);
	FMegalithBlueprintStructActions::RegisterActions(Registry);
	FMegalithBlueprintDataTableActions::RegisterActions(Registry);
	FMegalithBlueprintCurveTableActions::RegisterActions(Registry);
	FMegalithBlueprintStringTableActions::RegisterActions(Registry);
	FMegalithBlueprintBuildActions::RegisterActions(Registry);
	FMegalithBlueprintDiffActions::RegisterActions(Registry);
	FMegalithBlueprintTemplateActions::RegisterActions(Registry);
	FMegalithBlueprintGraphExportActions::RegisterActions(Registry);
	FMegalithBlueprintLayoutActions::RegisterActions(Registry);
	FMegalithBlueprintSpawnActions::RegisterActions(Registry);

	// Sprint 5 — Motion Matching Action Pack (Pillar D): character/actor BP scaffolding.
	FMegalithMotionMatchingScaffoldActions::RegisterActions(Registry);

	// Phase 1 bulk_fill / describe pilot adapter. Self-registers with
	// FMegalithBulkFillRegistry under namespace "blueprint"; routed-to by the
	// central bulk_fill.apply / describe.schema dispatchers (Phase 0).
	FMegalithBlueprintBulkFillAdapter::Register();

	UE_LOG(LogMegalith, Log, TEXT("Megalith — Blueprint module loaded (110 actions + bulk_fill/describe adapter)"));
}

void FMegalithBlueprintModule::ShutdownModule()
{
	FMegalithBlueprintBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("blueprint"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithBlueprintModule, MegalithBlueprint)
