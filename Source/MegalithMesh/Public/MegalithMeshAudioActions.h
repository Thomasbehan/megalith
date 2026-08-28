#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 8: Audio & Acoustics Actions (14 actions)
 * Material-aware spatial audio analysis, Sabine RT60 reverb estimation,
 * horror stealth mechanics, audio volume management.
 *
 * Read-Only (7): get_audio_volumes, get_surface_materials, estimate_footstep_sound,
 *   analyze_room_acoustics, analyze_sound_propagation, find_loud_surfaces, find_sound_paths
 * Horror AI (4): can_ai_hear_from, get_stealth_map, find_quiet_path, suggest_audio_volumes
 * Write (3): create_audio_volume, set_surface_type, create_surface_datatable
 */
class FMegalithMeshAudioActions
{
public:
	/** Register all 14 audio actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Read-Only ---
	static FMegalithActionResult GetAudioVolumes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSurfaceMaterials(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult EstimateFootstepSound(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeRoomAcoustics(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeSoundPropagation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindLoudSurfaces(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindSoundPaths(const TSharedPtr<FJsonObject>& Params);

	// --- Horror AI ---
	static FMegalithActionResult CanAiHearFrom(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetStealthMap(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindQuietPath(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SuggestAudioVolumes(const TSharedPtr<FJsonObject>& Params);

	// --- Write ---
	static FMegalithActionResult CreateAudioVolume(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSurfaceType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateSurfaceDataTable(const TSharedPtr<FJsonObject>& Params);
};
