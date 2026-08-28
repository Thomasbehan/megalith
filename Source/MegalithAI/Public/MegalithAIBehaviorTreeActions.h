#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIBehaviorTreeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// BT CRUD
	static FMegalithActionResult HandleCreateBehaviorTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBehaviorTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListBehaviorTrees(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteBehaviorTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateBehaviorTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetBTBlackboard(const TSharedPtr<FJsonObject>& Params);

	// Node discovery
	static FMegalithActionResult HandleListBTNodeClasses(const TSharedPtr<FJsonObject>& Params);

	// Node manipulation (Task 5)
	static FMegalithActionResult HandleAddBTNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveBTNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleMoveBTNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddBTDecorator(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveBTDecorator(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddBTService(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveBTService(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetBTNodeProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBTNodeProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReorderBTChildren(const TSharedPtr<FJsonObject>& Params);

	// Convenience tasks (Task 6)
	static FMegalithActionResult HandleAddBTRunEQSTask(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddBTSmartObjectTask(const TSharedPtr<FJsonObject>& Params);
	// Phase I2: BT-to-GAS direct ability activation
	static FMegalithActionResult HandleAddBTUseAbilityTask(const TSharedPtr<FJsonObject>& Params);

	// Spec-driven actions (Task 6)
	static FMegalithActionResult HandleBuildBTFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportBTSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportBTSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateBehaviorTree(const TSharedPtr<FJsonObject>& Params);

	// Phase F8 (J-phase): flat graph topology (parent_id + children GUIDs).
	// Differs from get_behavior_tree (recursive nested tree) — emits a flat
	// node array suitable for tools that need to look up a single node by GUID
	// without walking the full tree.
	static FMegalithActionResult HandleGetBTGraph(const TSharedPtr<FJsonObject>& Params);

	// Polish actions (Task 7)
	static FMegalithActionResult HandleCloneBTSubtree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAutoArrangeBT(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompareBehaviorTrees(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateBTTaskBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateBTDecoratorBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateBTServiceBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGenerateBTDiagram(const TSharedPtr<FJsonObject>& Params);
};
