#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Control Rig graph read/write action handlers for Megalith.
 * 3 actions: graph reading, node spawning, pin wiring.
 */
class MEGALITHANIMATION_API FMegalithControlRigWriteActions
{
public:
	/** Register all Control Rig graph actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleGetControlRigGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddControlRigNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConnectControlRigPins(const TSharedPtr<FJsonObject>& Params);
};
