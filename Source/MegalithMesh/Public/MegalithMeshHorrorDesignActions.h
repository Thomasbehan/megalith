#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 15: Horror Intelligence Actions (4 actions)
 * Higher-order horror design composition that builds on Phase 6 horror analysis:
 * player path prediction, spawn evaluation, scare positioning, encounter pacing.
 */
class FMegalithMeshHorrorDesignActions
{
public:
	/** Register all 4 horror intelligence actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	/** Generate weighted navmesh paths using multiple strategy heuristics */
	static FMegalithActionResult PredictPlayerPaths(const TSharedPtr<FJsonObject>& Params);

	/** Composite scoring of an enemy spawn location across visibility, lighting, audio, escape */
	static FMegalithActionResult EvaluateSpawnPoint(const TSharedPtr<FJsonObject>& Params);

	/** Find optimal positions for scripted scare events along a player path */
	static FMegalithActionResult SuggestScarePositions(const TSharedPtr<FJsonObject>& Params);

	/** Analyze encounter spacing, intensity curve, and rest periods along a level path */
	static FMegalithActionResult EvaluateEncounterPacing(const TSharedPtr<FJsonObject>& Params);
};
