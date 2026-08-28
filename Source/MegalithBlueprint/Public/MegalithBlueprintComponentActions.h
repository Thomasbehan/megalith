#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintComponentActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleAddComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReparentComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetComponentProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateComponent(const TSharedPtr<FJsonObject>& Params);
};
