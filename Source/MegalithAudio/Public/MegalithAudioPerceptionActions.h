#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * MCP action handlers for the Audio→AI design-time stimulus binding feature (Phase H3 / I3).
 *
 * Stamps UMegalithSoundPerceptionUserData onto USoundBase assets (Cue / MetaSoundSource / Wave)
 * via UE's canonical AssetUserData pattern. The runtime UMegalithAudioPerceptionSubsystem
 * (in MegalithAudioRuntime) reads this payload at play time and fires AActor::MakeNoise.
 *
 * Actions:
 *   audio::bind_sound_to_perception(asset_path, loudness, max_range, tag, sense_class, enabled,
 *                                   fire_on_fade_in, require_owning_actor)
 *   audio::unbind_sound_from_perception(asset_path)
 *   audio::get_sound_perception_binding(asset_path)
 *   audio::list_perception_bound_sounds()
 */
class FMegalithAudioPerceptionActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult BindSoundToPerception(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult UnbindSoundFromPerception(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundPerceptionBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListPerceptionBoundSounds(const TSharedPtr<FJsonObject>& Params);
};
