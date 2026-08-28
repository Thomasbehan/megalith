#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithAudioQueryActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult ListAudioAssets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SearchAudioAssets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundWaveInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundClassHierarchy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSubmixHierarchy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindAudioReferences(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindUnusedAudio(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindSoundsWithoutClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindUnattenuatedSounds(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetAudioStats(const TSharedPtr<FJsonObject>& Params);

	// Helpers
	static UClass* ResolveAudioClass(const FString& TypeName);
	static TSharedPtr<FJsonObject> BuildSoundClassTree(class USoundClass* SoundClass, TSet<USoundClass*>& Visited);
	static TSharedPtr<FJsonObject> BuildSubmixTree(class USoundSubmixBase* Submix, TSet<USoundSubmixBase*>& Visited);
	static FString CompressionTypeToString(uint8 Type);
};
