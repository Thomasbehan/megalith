#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FProjectListGameplayTagsAction
{
public:
	static FMegalithActionResult Execute(const TSharedPtr<FJsonObject>& Params);
	static FString GetName() { return TEXT("list_gameplay_tags"); }
	static FString GetDescription() { return TEXT("List all indexed gameplay tags, optionally filtered by prefix (e.g. 'Weapon.Melee')"); }
	static TSharedPtr<FJsonObject> GetSchema();
};
