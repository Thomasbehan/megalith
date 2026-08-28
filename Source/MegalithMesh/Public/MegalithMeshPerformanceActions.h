#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 9: Performance Analysis (5 actions)
 * Budget-aware placement analysis with conservative estimates.
 * No occlusion culling assumed — all estimates trend HIGH for safe budgeting.
 */
class FMegalithMeshPerformanceActions
{
public:
	/** Register all 5 performance analysis actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Region analysis ---
	static FMegalithActionResult GetRegionPerformance(const TSharedPtr<FJsonObject>& Params);

	// --- Pre-placement budgeting ---
	static FMegalithActionResult EstimatePlacementCost(const TSharedPtr<FJsonObject>& Params);

	// --- Overdraw detection ---
	static FMegalithActionResult FindOverdrawHotspots(const TSharedPtr<FJsonObject>& Params);

	// --- Shadow cost audit ---
	static FMegalithActionResult AnalyzeShadowCost(const TSharedPtr<FJsonObject>& Params);

	// --- Triangle budget ---
	static FMegalithActionResult GetTriangleBudget(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---
	static TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V);
};
