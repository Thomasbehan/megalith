#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverGraphActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Read (Phase 1)
	static FMegalithActionResult HandleGetSMStructure(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNodeDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNodeConnections(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindNodesByType(const TSharedPtr<FJsonObject>& Params);

	// Read (Phase 2)
	static FMegalithActionResult HandleFindNodesByClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSMStatistics(const TSharedPtr<FJsonObject>& Params);

	// Write (Phase 2)
	static FMegalithActionResult HandleAddState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddTransition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddConduit(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddStateMachineNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddAnyStateNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNodeProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetInitialState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEndState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNodeClass(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameNode(const TSharedPtr<FJsonObject>& Params);

	// Write (Phase 3)
	static FMegalithActionResult HandleMoveNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAutoArrangeGraph(const TSharedPtr<FJsonObject>& Params);

	// Compile (Phase 1)
	static FMegalithActionResult HandleCompileStateMachine(const TSharedPtr<FJsonObject>& Params);
};
