#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 21: Horror Advanced — Encounter Design Actions (8 actions)
 * High-level horror intelligence composing all existing analysis into
 * encounter design, patrol routing, territory analysis, safe room evaluation,
 * pacing structure, scare sequences, intensity validation, and hospice reports.
 *
 * Dependencies: Phase 6 (horror analysis), Phase 15 (horror design),
 *               Phase 7 (lighting), Phase 8 (acoustics), Phase 9 (accessibility)
 */
class FMegalithMeshEncounterActions
{
public:
	/** Register all 8 encounter/horror-advanced actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	/** Capstone: compose spawn + patrol + sightlines + audio into scored encounter spec */
	static FMegalithActionResult DesignEncounter(const TSharedPtr<FJsonObject>& Params);

	/** Generate navmesh patrol routes per AI archetype (stalker/patrol/ambusher) */
	static FMegalithActionResult SuggestPatrolRoute(const TSharedPtr<FJsonObject>& Params);

	/** Score a region as AI territory: hiding, patrol, ambush, sightline control */
	static FMegalithActionResult AnalyzeAiTerritory(const TSharedPtr<FJsonObject>& Params);

	/** Score a room as a safe room: entrances, lighting, sound isolation, defensibility */
	static FMegalithActionResult EvaluateSafeRoom(const TSharedPtr<FJsonObject>& Params);

	/** Macro-level tension mapping: encounter zones, safe rooms, pacing rhythm */
	static FMegalithActionResult AnalyzeLevelPacingStructure(const TSharedPtr<FJsonObject>& Params);

	/** Procedurally generate scare event sequence with variety, escalation, pacing */
	static FMegalithActionResult GenerateScareSequence(const TSharedPtr<FJsonObject>& Params);

	/** Validate horror intensity caps for hospice mode */
	static FMegalithActionResult ValidateHorrorIntensity(const TSharedPtr<FJsonObject>& Params);

	/** Full hospice accessibility + horror audit: intensity, rest, cognitive load, input demands */
	static FMegalithActionResult GenerateHospiceReport(const TSharedPtr<FJsonObject>& Params);
};
