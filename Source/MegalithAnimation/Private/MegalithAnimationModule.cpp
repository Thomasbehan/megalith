#include "MegalithAnimationModule.h"
#include "MegalithJsonUtils.h"
#include "MegalithAnimationActions.h"
#include "MegalithAnimationRuntimeActions.h"
#include "MegalithPoseSearchActions.h"
#include "MegalithMirrorTableActions.h"
#include "MegalithControlRigWriteActions.h"
#include "MegalithAbpWriteActions.h"
#include "MegalithAnimLayoutActions.h"
#include "MegalithAnimationBulkFillAdapter.h"
#include "MegalithChooserActions.h"
#include "MegalithChooserAuthoringActions.h"
#include "MegalithAbpGraphSurgeryActions.h"
#include "MegalithRetargetSettingsActions.h"
#include "MegalithSkeletonRetargetActions.h"
#include "MegalithLocomotionAuthoringActions.h"
#include "MegalithToolRegistry.h"

#define LOCTEXT_NAMESPACE "FMegalithAnimationModule"

void FMegalithAnimationModule::StartupModule()
{
	FMegalithAnimationActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithAnimationRuntimeActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithPoseSearchActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMirrorTableActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithControlRigWriteActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithAbpWriteActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithAnimLayoutActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithChooserActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithChooserAuthoringActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithAbpGraphSurgeryActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithRetargetSettingsActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithSkeletonRetargetActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithLocomotionAuthoringActions::RegisterActions(FMegalithToolRegistry::Get());

	// Phase 5 Step 6 (MCP Ergonomics, 2026-05-11) — register the animation adapter.
	// PoseSearchDatabase fill_kind replaces the 40+ add_database_animation
	// round-trips per locomotion set (design B.3 pain point).
	FMegalithAnimationBulkFillAdapter::Register();

	UE_LOG(LogMegalith, Verbose, TEXT("Megalith - Animation module loaded"));
}

void FMegalithAnimationModule::ShutdownModule()
{
	FMegalithAnimationBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("animation"));
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("chooser"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithAnimationModule, MegalithAnimation)
