#pragma once

#include "MegalithAIInternal.h"

class FMegalithAISmartObjectActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

#if WITH_SMARTOBJECTS
private:
	// Definition CRUD
	static FMegalithActionResult HandleCreateSmartObjectDefinition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSmartObjectDefinition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListSmartObjectDefinitions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteSmartObjectDefinition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateSmartObjectDefinition(const TSharedPtr<FJsonObject>& Params);

	// Slot management
	static FMegalithActionResult HandleAddSOSlot(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSOSlot(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureSOSlot(const TSharedPtr<FJsonObject>& Params);

	// Behavior definitions
	static FMegalithActionResult HandleAddSOBehaviorDefinition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSOBehaviorDefinition(const TSharedPtr<FJsonObject>& Params);

	// Tags
	static FMegalithActionResult HandleSetSOTags(const TSharedPtr<FJsonObject>& Params);

	// Component / Placement
	static FMegalithActionResult HandleAddSmartObjectComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandlePlaceSmartObjectActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindSmartObjectsInLevel(const TSharedPtr<FJsonObject>& Params);

	// Utilities
	static FMegalithActionResult HandleValidateSmartObjectDefinition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateSOFromTemplate(const TSharedPtr<FJsonObject>& Params);

	// ── Helpers ──
	static class USmartObjectDefinition* LoadSODefinition(const TSharedPtr<FJsonObject>& Params, FString& OutAssetPath, FString& OutError);
	static TSharedPtr<FJsonObject> SlotToJson(const struct FSmartObjectSlotDefinition& Slot, int32 Index);
	static TSharedPtr<FJsonObject> DefinitionToJson(class USmartObjectDefinition* Def, const FString& AssetPath);
	static void ParseTagContainer(const TSharedPtr<FJsonObject>& Params, const FString& FieldName, struct FGameplayTagContainer& OutTags);
#endif // WITH_SMARTOBJECTS
};
