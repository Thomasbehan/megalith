#include "MegalithMeshModule.h"
#include "MegalithMeshInspectionActions.h"
#include "MegalithMeshSceneActions.h"
#include "MegalithMeshSpatialActions.h"
#include "MegalithMeshBlockoutActions.h"
#include "MegalithMeshHorrorActions.h"
#include "MegalithMeshAccessibilityActions.h"
#include "MegalithMeshPerformanceActions.h"
#include "MegalithMeshLightingActions.h"
#include "MegalithMeshDecalActions.h"
#include "MegalithMeshAudioActions.h"
#include "MegalithMeshTemplateActions.h"
#include "MegalithMeshLevelDesignActions.h"
#include "MegalithMeshVolumeActions.h"
#include "MegalithMeshTechArtActions.h"
#include "MegalithMeshHorrorDesignActions.h"
#include "MegalithMeshAdvancedLevelActions.h"
#include "MegalithMeshContextPropActions.h"
#include "MegalithMeshPresetActions.h"
#include "MegalithMeshEncounterActions.h"
#include "MegalithMeshQualityActions.h"
#include "MegalithMeshFloorPlanGenerator.h"
#include "MegalithMeshSpatialRegistry.h"
#include "MegalithMeshAutoVolumeActions.h"
#include "MegalithMeshFurnishingActions.h"
#include "MegalithMeshDebugViewActions.h"
#include "MegalithMeshBuildingValidationActions.h"
#include "MegalithMeshBulkFillAdapter.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithSettings.h"
#include "Misc/CoreDelegates.h"

#if WITH_GEOMETRYSCRIPT
#include "MegalithMeshOperationActions.h"
#include "MegalithMeshProceduralActions.h"
#include "MegalithMeshBuildingActions.h"
#include "MegalithMeshFacadeActions.h"
#include "MegalithMeshRoofActions.h"
#include "MegalithMeshCityBlockActions.h"
#include "MegalithMeshTerrainActions.h"
#include "MegalithMeshArchFeatureActions.h"
#include "MegalithMeshHandlePool.h"
#endif

#define LOCTEXT_NAMESPACE "FMegalithMeshModule"

void FMegalithMeshModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableMesh)
	{
		UE_LOG(LogMegalith, Log, TEXT("Megalith — Mesh module disabled via settings"));
		return;
	}

	FMegalithMeshInspectionActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshSceneActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshSpatialActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshBlockoutActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshHorrorActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshAccessibilityActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshPerformanceActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshLightingActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshDecalActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshAudioActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshTemplateActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshLevelDesignActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshVolumeActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshTechArtActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshHorrorDesignActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshAdvancedLevelActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshContextPropActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshPresetActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshEncounterActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshQualityActions::RegisterActions(FMegalithToolRegistry::Get());
	// --- Procedural Town Generation (experimental, off by default) ---
	if (GetDefault<UMegalithSettings>()->bEnableProceduralTownGen)
	{
		FMegalithMeshFloorPlanGenerator::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshSpatialRegistry::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshAutoVolumeActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshFurnishingActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshDebugViewActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshBuildingValidationActions::RegisterActions(FMegalithToolRegistry::Get());
	}

#if WITH_GEOMETRYSCRIPT
	HandlePool = NewObject<UMegalithMeshHandlePool>();
	HandlePool->AddToRoot();
	HandlePool->Initialize();
	FMegalithMeshOperationActions::SetHandlePool(HandlePool);
	FMegalithMeshOperationActions::RegisterActions(FMegalithToolRegistry::Get());
	FMegalithMeshProceduralActions::SetHandlePool(HandlePool);
	FMegalithMeshProceduralActions::RegisterActions(FMegalithToolRegistry::Get());

	// --- Town gen GeometryScript actions (experimental, off by default) ---
	if (GetDefault<UMegalithSettings>()->bEnableProceduralTownGen)
	{
		FMegalithMeshBuildingActions::SetHandlePool(HandlePool);
		FMegalithMeshBuildingActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshFacadeActions::SetHandlePool(HandlePool);
		FMegalithMeshFacadeActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshRoofActions::SetHandlePool(HandlePool);
		FMegalithMeshRoofActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshCityBlockActions::SetHandlePool(HandlePool);
		FMegalithMeshCityBlockActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshTerrainActions::SetHandlePool(HandlePool);
		FMegalithMeshTerrainActions::RegisterActions(FMegalithToolRegistry::Get());
		FMegalithMeshArchFeatureActions::SetHandlePool(HandlePool);
		FMegalithMeshArchFeatureActions::RegisterActions(FMegalithToolRegistry::Get());
	}

	FMegalithMeshTechArtActions::SetHandlePool(HandlePool);

	// Clean up handle pool on PreExit — before GC destroys UObjects.
	// ShutdownModule runs too late; by then the UObject array may be torn down.
	FCoreDelegates::OnPreExit.AddLambda([this]()
	{
		if (HandlePool && HandlePool->IsValidLowLevelFast())
		{
			HandlePool->Teardown();
			HandlePool->RemoveFromRoot();
			FMegalithMeshOperationActions::SetHandlePool(nullptr);
			FMegalithMeshProceduralActions::SetHandlePool(nullptr);
			FMegalithMeshBuildingActions::SetHandlePool(nullptr);
			FMegalithMeshFacadeActions::SetHandlePool(nullptr);
			FMegalithMeshRoofActions::SetHandlePool(nullptr);
			FMegalithMeshCityBlockActions::SetHandlePool(nullptr);
			FMegalithMeshTerrainActions::SetHandlePool(nullptr);
			FMegalithMeshArchFeatureActions::SetHandlePool(nullptr);
			FMegalithMeshTechArtActions::SetHandlePool(nullptr);
			HandlePool = nullptr;
		}
	});

	UE_LOG(LogMegalith, Log, TEXT("Megalith — Mesh operations enabled (GeometryScript available)"));
#endif

	// Phase 5 Step 5 (MCP Ergonomics, 2026-05-11) — register the mesh adapter
	// OUTSIDE the WITH_GEOMETRYSCRIPT gate so bulk_fill is available regardless
	// of GeometryScript availability. SurfaceDataTable + ActorProperties
	// fill_kinds are reflection-bound, not GeometryScript-bound.
	FMegalithMeshBulkFillAdapter::Register();

	UE_LOG(LogMegalith, Log, TEXT("Megalith — Mesh module loaded (%d actions)"),
		FMegalithToolRegistry::Get().GetActions(TEXT("mesh")).Num());
}

void FMegalithMeshModule::ShutdownModule()
{
	// Handle pool cleanup happens in OnPreExit (before GC destroys UObjects).
	// By the time ShutdownModule runs, the UObject array may already be torn down.
	// Just null our pointer defensively.
#if WITH_GEOMETRYSCRIPT
	HandlePool = nullptr;
#endif

	FMegalithMeshBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("mesh"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithMeshModule, MegalithMesh)
