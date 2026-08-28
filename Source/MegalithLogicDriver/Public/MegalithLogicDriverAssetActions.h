#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverAssetActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Asset CRUD
	static FMegalithActionResult HandleCreateStateMachine(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetStateMachine(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListStateMachines(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteStateMachine(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateStateMachine(const TSharedPtr<FJsonObject>& Params);

	// Node Blueprints (Phase 2)
	static FMegalithActionResult HandleCreateNodeBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNodeBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListNodeBlueprints(const TSharedPtr<FJsonObject>& Params);
};
