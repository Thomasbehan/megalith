#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectSearchAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("search"); }
	static FString GetDescription() { return TEXT("Full-text search across indexed project assets (name, class, description, path, module) and graph nodes (name, class, type)"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
