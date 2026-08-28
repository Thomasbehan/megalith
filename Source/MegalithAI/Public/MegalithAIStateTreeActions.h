#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIStateTreeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

#if WITH_STATETREE
private:
	// CRUD
	static FMegalithActionResult HandleCreateStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListStateTrees(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompileStateTree(const TSharedPtr<FJsonObject>& Params);

	// Schema
	static FMegalithActionResult HandleSetSTSchema(const TSharedPtr<FJsonObject>& Params);

	// State management
	static FMegalithActionResult HandleAddSTState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSTState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameSTState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleMoveSTState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSTStateProperties(const TSharedPtr<FJsonObject>& Params);

	// Tasks
	static FMegalithActionResult HandleAddSTTask(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSTTask(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSTTaskProperty(const TSharedPtr<FJsonObject>& Params);

	// Enter conditions
	static FMegalithActionResult HandleAddSTEnterCondition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSTEnterCondition(const TSharedPtr<FJsonObject>& Params);

	// Transitions
	static FMegalithActionResult HandleAddSTTransition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSTTransition(const TSharedPtr<FJsonObject>& Params);

	// Bindings
	static FMegalithActionResult HandleAddSTPropertyBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSTPropertyBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSTBindings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSTBindableProperties(const TSharedPtr<FJsonObject>& Params);

	// Discovery
	static FMegalithActionResult HandleListSTTaskTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListSTConditionTypes(const TSharedPtr<FJsonObject>& Params);

	// Advanced: transition conditions, considerations, validation, extensions
	static FMegalithActionResult HandleAddSTTransitionCondition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSTConsideration(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureSTConsideration(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateStateTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListSTExtensionTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSTExtension(const TSharedPtr<FJsonObject>& Params);

	// Declarative build, export, diagram, auto-layout
	static FMegalithActionResult HandleBuildStateTreeFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportSTSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGenerateSTDiagram(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAutoArrangeST(const TSharedPtr<FJsonObject>& Params);
#endif
};
