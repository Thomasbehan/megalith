#pragma once

#include "CoreMinimal.h"

struct FMegalithActionResult;
class FMegalithToolRegistry;

class FMegalithAudioBatchActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult BatchAssignSoundClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchAssignAttenuation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetCompression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetSubmix(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetConcurrency(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetLooping(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetVirtualization(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchRenameAudio(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchSetSoundWaveProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ApplyAudioTemplate(const TSharedPtr<FJsonObject>& Params);
};
