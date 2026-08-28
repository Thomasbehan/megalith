#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithNiagaraTimingActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// System-level (4)
	static FMegalithActionResult HandleGetSystemTiming(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetWarmupProfile(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetFixedTickDelta(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRequireCurrentFrameData(const TSharedPtr<FJsonObject>& Params);

	// Emitter composite + read aggregator (2)
	static FMegalithActionResult HandleSetEmitterLoopProfile(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEmitterTimingSummary(const TSharedPtr<FJsonObject>& Params);

	// Sim-stage aliases (2)
	static FMegalithActionResult HandleSetSimStageIterationCount(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSimStageExecuteBehavior(const TSharedPtr<FJsonObject>& Params);

	// Particle lifetime (1)
	static FMegalithActionResult HandleSetParticleLifetime(const TSharedPtr<FJsonObject>& Params);
};
