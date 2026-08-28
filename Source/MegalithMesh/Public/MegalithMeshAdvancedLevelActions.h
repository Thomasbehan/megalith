#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 17: Advanced Level Design (8 actions)
 * Sublevels, BP actor spawning, splines, prefabs (Level Instances),
 * randomize transforms, filtered actor enumeration, distance measurement.
 *
 * set_collision_preset already exists in Phase 14 (VolumeActions) -- skipped here.
 */
class FMegalithMeshAdvancedLevelActions
{
public:
	/** Register all advanced level design actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	/** Create/load/unload sublevels, move actors between levels */
	static FMegalithActionResult ManageSublevel(const TSharedPtr<FJsonObject>& Params);

	/** Spawn a Blueprint actor with exposed property configuration */
	static FMegalithActionResult PlaceBlueprintActor(const TSharedPtr<FJsonObject>& Params);

	/** Spawn an actor with a USplineComponent + optional USplineMeshComponents per segment */
	static FMegalithActionResult PlaceSpline(const TSharedPtr<FJsonObject>& Params);

	/** Create a Level Instance (prefab) from selected actors */
	static FMegalithActionResult CreatePrefab(const TSharedPtr<FJsonObject>& Params);

	/** Create a Blueprint prefab from world actors (dialog-free, MCP-safe) */
	static FMegalithActionResult CreateBlueprintPrefab(const TSharedPtr<FJsonObject>& Params);

	/** Spawn a copy of a saved Level Instance prefab */
	static FMegalithActionResult SpawnPrefab(const TSharedPtr<FJsonObject>& Params);

	/** Apply random offset/rotation/scale variation to actors */
	static FMegalithActionResult RandomizeTransforms(const TSharedPtr<FJsonObject>& Params);

	/** Filtered enumeration of actors in the editor world */
	static FMegalithActionResult GetLevelActors(const TSharedPtr<FJsonObject>& Params);

	/** Measure distance between two actors or world points */
	static FMegalithActionResult MeasureDistance(const TSharedPtr<FJsonObject>& Params);
};
