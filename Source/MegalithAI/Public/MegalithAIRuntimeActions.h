#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIRuntimeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Blackboard (167-169)
	static FMegalithActionResult HandleRuntimeGetBBValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeSetBBValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeClearBBValue(const TSharedPtr<FJsonObject>& Params);

	// Behavior Tree (170-173)
	static FMegalithActionResult HandleRuntimeGetBTState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeStartBT(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeStopBT(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeGetBTExecutionPath(const TSharedPtr<FJsonObject>& Params);

	// Perception (174-176)
	static FMegalithActionResult HandleRuntimeGetPerceivedActors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeCheckPerception(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeReportNoise(const TSharedPtr<FJsonObject>& Params);

	// StateTree (177-178)
	static FMegalithActionResult HandleRuntimeGetSTActiveStates(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRuntimeSendSTEvent(const TSharedPtr<FJsonObject>& Params);

	// Smart Objects (179)
	static FMegalithActionResult HandleRuntimeFindSmartObjects(const TSharedPtr<FJsonObject>& Params);

	// EQS (180)
	static FMegalithActionResult HandleRuntimeRunEQSQuery(const TSharedPtr<FJsonObject>& Params);
};
