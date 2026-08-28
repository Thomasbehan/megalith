#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * PoseSearch domain action handlers for Megalith.
 * 5 actions: schema inspection, database CRUD, stats.
 */
class MEGALITHANIMATION_API FMegalithPoseSearchActions
{
public:
	/** Register all PoseSearch actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleGetPoseSearchSchema(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetPoseSearchDatabase(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddDatabaseSequence(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveDatabaseSequence(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDatabaseStats(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 11: PoseSearch Creation (2) ---
	static FMegalithActionResult HandleCreatePoseSearchSchema(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreatePoseSearchDatabase(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 14: PoseSearch Writes (6) ---
	static FMegalithActionResult HandleSetDatabaseSequenceProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSchemaChannel(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSchemaChannel(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetChannelWeight(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRebuildPoseSearchIndex(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDatabaseSearchMode(const TSharedPtr<FJsonObject>& Params);

	// --- Motion Matching Pack Sprint 1: NormalizationSet + DB entry tags (3 class-member) ---
	static FMegalithActionResult HandleCreateNormalizationSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddDatabaseToNormalizationSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDatabaseEntryTags(const TSharedPtr<FJsonObject>& Params);
};
