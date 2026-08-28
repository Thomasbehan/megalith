#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintBuildActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleBuildBlueprintFromSpec(const TSharedPtr<FJsonObject>& Params);
};
