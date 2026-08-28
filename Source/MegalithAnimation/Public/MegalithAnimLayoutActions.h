#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Animation Blueprint auto-layout action for Megalith.
 * Uses IMegalithGraphFormatter (Blueprint Assist bridge) to format ABP graphs.
 */
class MEGALITHANIMATION_API FMegalithAnimLayoutActions
{
public:
	/** Register all layout actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleAutoLayout(const TSharedPtr<FJsonObject>& Params);
};
