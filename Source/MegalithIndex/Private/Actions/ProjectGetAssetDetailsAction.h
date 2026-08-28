#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectGetAssetDetailsAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("get_asset_details"); }
	static FString GetDescription() { return TEXT("Get deep details for a specific asset -- nodes, variables, parameters, dependencies"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
