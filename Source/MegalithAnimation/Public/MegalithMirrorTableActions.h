#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * MirrorDataTable domain action handlers for Megalith (Motion Matching Pack — Sprint 2).
 * - create_mirror_data_table     : create a UMirrorDataTable, populate find/replace rules, build rows.
 * - set_schema_mirror_data_table : assign a mirror table to a PoseSearchSchema roled-skeleton slot.
 */
class MEGALITHANIMATION_API FMegalithMirrorTableActions
{
public:
	/** Register all mirror-table actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleCreateMirrorDataTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSchemaMirrorDataTable(const TSharedPtr<FJsonObject>& Params);
};
