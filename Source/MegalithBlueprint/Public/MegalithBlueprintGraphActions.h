#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintGraphActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleAddFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleOverrideParentFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetFunctionThreadSafe(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddMacro(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveMacro(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameMacro(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddEventDispatcher(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetFunctionParams(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImplementInterface(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveInterface(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReparentBlueprint(const TSharedPtr<FJsonObject>& Params);

	// Wave 5 — Scaffolding & Templates
	static FMegalithActionResult HandleScaffoldInterfaceImplementation(const TSharedPtr<FJsonObject>& Params);

	// Wave 6 — Event Dispatcher CRUD
	static FMegalithActionResult HandleRemoveEventDispatcher(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEventDispatcherParams(const TSharedPtr<FJsonObject>& Params);
};
