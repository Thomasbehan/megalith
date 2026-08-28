#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectCleanupGeneratedAssetsAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("cleanup_generated_assets"); }
	static FString GetDescription() { return TEXT("Safely delete generated throwaway assets under /Game/Tests/Megalith/ with reference checks and optional empty-folder removal; dry-run by default"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
