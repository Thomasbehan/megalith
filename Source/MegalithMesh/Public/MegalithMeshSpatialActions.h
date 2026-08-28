#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 3: Scene Spatial Queries (11 actions)
 * Physics-based world queries - raycasts, overlaps, spatial relationships.
 * Enables AI spatial reasoning about the scene.
 * All queries work in editor without a play session.
 */
class FMegalithMeshSpatialActions
{
public:
	/** Register all 11 spatial query actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Raycasts ---
	static FMegalithActionResult QueryRaycast(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult QueryMultiRaycast(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult QueryRadialSweep(const TSharedPtr<FJsonObject>& Params);

	// --- Overlaps ---
	static FMegalithActionResult QueryOverlap(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult QueryNearest(const TSharedPtr<FJsonObject>& Params);

	// --- Line of sight ---
	static FMegalithActionResult QueryLineOfSight(const TSharedPtr<FJsonObject>& Params);

	// --- Volume / bounds ---
	static FMegalithActionResult GetActorsInVolume(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSceneBounds(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSceneStatistics(const TSharedPtr<FJsonObject>& Params);

	// --- Spatial analysis ---
	static FMegalithActionResult GetSpatialRelationships(const TSharedPtr<FJsonObject>& Params);

	// --- Navigation ---
	static FMegalithActionResult QueryNavmesh(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---
	static ECollisionChannel ParseCollisionChannel(const FString& ChannelName, bool& bSuccess);
	static TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V);
	static TSharedPtr<FJsonObject> HitResultToJson(const FHitResult& Hit);
};
