#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASTagActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Tag CRUD
	static FMegalithActionResult HandleAddGameplayTags(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetTagHierarchy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchTagUsage(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Productivity
	static FMegalithActionResult HandleScaffoldTagHierarchy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameTag(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveGameplayTags(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateTagConsistency(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Advanced
	static FMegalithActionResult HandleAuditTagNaming(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportTagHierarchy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportTagHierarchy(const TSharedPtr<FJsonObject>& Params);
};
