#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 6: Horror Analysis Actions (8 actions)
 * Spatial analysis for survival horror level design:
 * sightlines, hiding spots, ambush points, choke points,
 * escape routes, tension classification, pacing curves, dead ends.
 */
class FMegalithMeshHorrorActions
{
public:
	/** Register all 8 horror analysis actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult AnalyzeSightlines(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindHidingSpots(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindAmbushPoints(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeChokePoints(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeEscapeRoutes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ClassifyZoneTension(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzePacingCurve(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindDeadEnds(const TSharedPtr<FJsonObject>& Params);
};
