// Copyright Megalith. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintSpawnActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleSpawnBlueprintActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchSpawnBlueprintActors(const TSharedPtr<FJsonObject>& Params);
};
