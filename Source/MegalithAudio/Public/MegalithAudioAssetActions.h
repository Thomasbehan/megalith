#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithAudioAssetActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Sound Attenuation ---
	static FMegalithActionResult CreateSoundAttenuation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetAttenuationSettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetAttenuationSettings(const TSharedPtr<FJsonObject>& Params);

	// --- Sound Class ---
	static FMegalithActionResult CreateSoundClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundClassProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSoundClassProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Sound Mix ---
	static FMegalithActionResult CreateSoundMix(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundMixSettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSoundMixSettings(const TSharedPtr<FJsonObject>& Params);

	// --- Sound Concurrency ---
	static FMegalithActionResult CreateSoundConcurrency(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetConcurrencySettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetConcurrencySettings(const TSharedPtr<FJsonObject>& Params);

	// --- Sound Submix ---
	static FMegalithActionResult CreateSoundSubmix(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSubmixProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSubmixProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Test fixtures (F18) ---
	/** Procedurally synthesizes a 16-bit mono sine-tone USoundWave. Zero asset dependencies. */
	static FMegalithActionResult CreateTestWave(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---

	/** Split "/Game/Foo/Bar" into PackagePath="/Game/Foo" and AssetName="Bar" */
	static bool SplitAssetPath(const FString& AssetPath, FString& OutPackagePath, FString& OutAssetName);

	/** Load an audio asset by path, using AssetRegistry first, then StaticLoadObject fallback */
	template<typename T>
	static T* LoadAudioAsset(const FString& AssetPath, FString& OutError);

	/** Serialize all UPROPERTY fields of a UStruct instance to a JSON object via reflection */
	static TSharedPtr<FJsonObject> StructToJson(const UStruct* StructDef, const void* StructData);

	/** Apply JSON fields onto a UStruct instance via reflection (partial update) */
	static bool JsonToStruct(const TSharedPtr<FJsonObject>& Json, const UStruct* StructDef, void* StructData, FString& OutError);

	/** Create asset via factory, register, save, return package */
	template<typename TFactory, typename TAsset>
	static TAsset* CreateAudioAsset(const FString& AssetPath, FString& OutError);
};
