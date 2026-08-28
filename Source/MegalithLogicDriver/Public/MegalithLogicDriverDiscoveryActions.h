#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverDiscoveryActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Discovery/Inspection (Phase 1 partial, rest Phase 2-4)
	static FMegalithActionResult HandleGetSMOverview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateStateMachine(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindSMReferences(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindNodeClassUsages(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleVisualizeSMAsText(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExplainStateMachine(const TSharedPtr<FJsonObject>& Params);
};
