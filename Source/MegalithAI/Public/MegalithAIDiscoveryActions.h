#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIDiscoveryActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleGetAIOverview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAINodeTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchAIAssets(const TSharedPtr<FJsonObject>& Params);

	// Cross-reference discovery
	static FMegalithActionResult HandleValidateAIDataFlow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindEQSReferences(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindSOReferences(const TSharedPtr<FJsonObject>& Params);

	// Lint & manifest
	static FMegalithActionResult HandleLintBehaviorTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleLintStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDetectAICircularReferences(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportAIManifest(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAIBehaviorSummary(const TSharedPtr<FJsonObject>& Params);
};
