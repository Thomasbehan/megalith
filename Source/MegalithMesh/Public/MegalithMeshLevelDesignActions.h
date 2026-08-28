#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 13: Level Design Quick Wins (9 actions)
 * Lights, materials, mesh swap, LOD, instancing, component property reflection.
 * High-frequency actions for level design sessions.
 */
class FMegalithMeshLevelDesignActions
{
public:
	/** Register all 9 level design actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	/** Spawn point/spot/rect/directional light with full properties */
	static FMegalithActionResult PlaceLight(const TSharedPtr<FJsonObject>& Params);

	/** Modify properties on an existing light actor */
	static FMegalithActionResult SetLightProperties(const TSharedPtr<FJsonObject>& Params);

	/** Assign a material to an actor's mesh component by slot index or name */
	static FMegalithActionResult SetActorMaterial(const TSharedPtr<FJsonObject>& Params);

	/** Bulk replace material X with Y across actors or entire level */
	static FMegalithActionResult SwapMaterialInLevel(const TSharedPtr<FJsonObject>& Params);

	/** Swap all instances of static mesh X with mesh Y */
	static FMegalithActionResult FindReplaceMesh(const TSharedPtr<FJsonObject>& Params);

	/** Set per-LOD screen size thresholds on a static mesh asset */
	static FMegalithActionResult SetLodScreenSizes(const TSharedPtr<FJsonObject>& Params);

	/** Identify meshes used many times that could be HISM-converted */
	static FMegalithActionResult FindInstancingCandidates(const TSharedPtr<FJsonObject>& Params);

	/** Convert grouped StaticMeshActors into a single HISM actor */
	static FMegalithActionResult ConvertToHism(const TSharedPtr<FJsonObject>& Params);

	/** Read arbitrary component properties via FProperty reflection */
	static FMegalithActionResult GetActorComponentProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---

	/** Apply light properties from JSON to a light component. Returns list of properties set. */
	static TArray<FString> ApplyLightProperties(class ULightComponent* LightComp, const TSharedPtr<FJsonObject>& Params);
};
