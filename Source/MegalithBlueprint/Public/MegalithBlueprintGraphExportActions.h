#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintGraphExportActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// Phase 5C — Graph export/import/copy
	static FMegalithActionResult HandleExportGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCopyNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateGraph(const TSharedPtr<FJsonObject>& Params);
};
