#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverRuntimeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Runtime inspection/control (Phase 3)
	static FMegalithActionResult HandleRuntimeGetSMState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeStartSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeStopSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeRestartSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeSwitchState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeEvaluateTransitions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeGetStateHistory(const TSharedPtr<FJsonObject>& Params);
};
