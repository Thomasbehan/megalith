#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverNodeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Node configuration (Phase 2-3)
	static FMegalithActionResult HandleConfigureState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureTransition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureConduit(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetTransitionCondition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetExposedProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetExposedProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureStateMachineNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetStateTags(const TSharedPtr<FJsonObject>& Params);
};
