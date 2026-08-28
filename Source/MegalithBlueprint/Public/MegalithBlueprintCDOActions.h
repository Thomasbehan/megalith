#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintCDOActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleGetCDOProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCDOProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetPropertyAtPath(const TSharedPtr<FJsonObject>& Params);
};
