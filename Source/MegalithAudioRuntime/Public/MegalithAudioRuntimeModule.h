#pragma once

#include "Modules/ModuleInterface.h"
#include "Logging/LogMacros.h"

MEGALITHAUDIORUNTIME_API DECLARE_LOG_CATEGORY_EXTERN(LogMegalithAudioRuntime, Log, All);

/**
 * Runtime-typed Megalith sub-module hosting the audio→AI perception subsystem.
 *
 * Hosts:
 *  - UMegalithSoundPerceptionUserData     (UAssetUserData payload stamped onto USoundBase)
 *  - UMegalithAudioPerceptionSubsystem    (UWorldSubsystem hooks UAudioComponent state changes)
 *  - UMegalithAudioPerceptionStatics      (UBlueprintFunctionLibrary fire-and-forget helper)
 *
 * Editor-side action handlers (audio::bind_sound_to_perception, ...) live in MegalithAudio
 * (Editor-typed) and depend publicly on this module to access the UserData class.
 */
class FMegalithAudioRuntimeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
