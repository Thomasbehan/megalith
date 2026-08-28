#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIBlackboardActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Blackboard CRUD
	static FMegalithActionResult HandleCreateBlackboard(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBlackboard(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListBlackboards(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteBlackboard(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateBlackboard(const TSharedPtr<FJsonObject>& Params);

	// Key management
	static FMegalithActionResult HandleAddBBKey(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveBBKey(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameBBKey(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBBKeyDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchAddBBKeys(const TSharedPtr<FJsonObject>& Params);

	// Parent / comparison
	static FMegalithActionResult HandleSetBBParent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompareBlackboards(const TSharedPtr<FJsonObject>& Params);
};
