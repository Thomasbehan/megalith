#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectRefreshAssetsAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("refresh_assets"); }
	static FString GetDescription() { return TEXT("Force a synchronous asset-registry rescan of requested paths (post-save freshness); optionally wait for the backing files to land on disk"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
