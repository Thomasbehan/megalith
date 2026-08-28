#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithComboGraphActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Read/Inspect
	static FMegalithActionResult HandleListComboGraphs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetComboGraphInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetComboNodeEffects(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateComboGraph(const TSharedPtr<FJsonObject>& Params);

	// Create/Modify
	static FMegalithActionResult HandleCreateComboGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddComboNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddComboEdge(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetComboNodeEffects(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetComboNodeCues(const TSharedPtr<FJsonObject>& Params);

	// Scaffolding
	static FMegalithActionResult HandleCreateComboAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleLinkAbilityToComboGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldComboFromMontages(const TSharedPtr<FJsonObject>& Params);

	// Layout
	static FMegalithActionResult HandleLayoutComboGraph(const TSharedPtr<FJsonObject>& Params);
};
