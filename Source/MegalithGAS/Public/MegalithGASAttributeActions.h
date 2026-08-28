#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASAttributeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Core CRUD
	static FMegalithActionResult HandleCreateAttributeSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddAttribute(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAttributeSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAttributeDefaults(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAttributeSets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureAttributeClamping(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureMetaAttributes(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Productivity
	static FMegalithActionResult HandleCreateAttributeSetFromTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateAttributeInitDataTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateAttributeSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureAttributeReplication(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleLinkDataTableToASC(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBulkEditAttributes(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Analysis
	static FMegalithActionResult HandleValidateAttributeSet(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindAttributeModifiers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDiffAttributeSets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAttributeDependencyGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveAttribute(const TSharedPtr<FJsonObject>& Params);
	// Phase 4: Runtime
	static FMegalithActionResult HandleGetAttributeValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAttributeValue(const TSharedPtr<FJsonObject>& Params);
};
