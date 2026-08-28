#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectFindReferencesAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("find_references"); }
	static FString GetDescription() { return TEXT("Find all assets that reference or are referenced by the given asset"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
