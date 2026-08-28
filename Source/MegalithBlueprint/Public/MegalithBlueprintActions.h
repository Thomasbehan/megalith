#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintActions
{
public:
	static void RegisterActions();

	static FMegalithActionResult HandleListGraphs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetGraphData(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetGraphSummary(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetVariables(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetExecutionFlow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetComponents(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetComponentDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetFunctions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEventDispatchers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetParentClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetInterfaces(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetConstructionScript(const TSharedPtr<FJsonObject>& Params);

	// Wave 3 — Discovery & Resolution
	static FMegalithActionResult HandleSearchFunctions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNodeDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetInterfaceFunctions(const TSharedPtr<FJsonObject>& Params);

	// Wave 6 — Inspection & Editing
	static FMegalithActionResult HandleGetFunctionSignature(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEventDispatcherDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBlueprintInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindVariableReferences(const TSharedPtr<FJsonObject>& Params);

private:
	static UBlueprint* LoadBlueprint(const TSharedPtr<FJsonObject>& Params, FString& OutAssetPath);
};
