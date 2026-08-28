#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 7: Lighting Analysis Actions (5 actions)
 * Hybrid scene capture + analytic lighting analysis.
 * Measures actual illumination including Lumen GI via scene capture,
 * with analytic fallback for light attribution (which light dominates a point).
 */
class FMegalithMeshLightingActions
{
public:
	/** Register all 5 lighting analysis actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	/** Sample light levels at specified points (capture/analytic/both) */
	static FMegalithActionResult SampleLightLevels(const TSharedPtr<FJsonObject>& Params);

	/** Find contiguous dark regions in a volume */
	static FMegalithActionResult FindDarkCorners(const TSharedPtr<FJsonObject>& Params);

	/** Analyze light transitions along a path, flag harsh changes */
	static FMegalithActionResult AnalyzeLightTransitions(const TSharedPtr<FJsonObject>& Params);

	/** Room-level lighting coverage audit */
	static FMegalithActionResult GetLightCoverage(const TSharedPtr<FJsonObject>& Params);

	/** Suggest light placements for a given mood */
	static FMegalithActionResult SuggestLightPlacement(const TSharedPtr<FJsonObject>& Params);
};
