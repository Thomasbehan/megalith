#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintVariableActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleAddVariable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveVariable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameVariable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetVariableType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetVariableDefaults(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddLocalVariable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveLocalVariable(const TSharedPtr<FJsonObject>& Params);

	// Wave 7 — Advanced
	static FMegalithActionResult HandleAddReplicatedVariable(const TSharedPtr<FJsonObject>& Params);
};
