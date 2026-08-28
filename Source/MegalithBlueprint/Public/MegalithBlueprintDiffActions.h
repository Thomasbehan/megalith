#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintDiffActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleCompareBlueprints(const TSharedPtr<FJsonObject>& Params);
};
