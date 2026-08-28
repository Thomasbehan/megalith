#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASAbilityActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Core CRUD
	static FMegalithActionResult HandleCreateAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAbilities(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompileAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityTags(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityTags(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityPolicy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityCost(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityCooldown(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityTriggers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAbilityFlags(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Graph Building + Templates
	static FMegalithActionResult HandleAddAbilityTaskNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddCommitAndEndFlow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddEffectApplication(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddGameplayCueNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateAbilityFromTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildAbilityFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchCreateAbilities(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateAbility(const TSharedPtr<FJsonObject>& Params);
	// Phase 2: Ability Tasks
	static FMegalithActionResult HandleListAbilityTasks(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityTaskPins(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleWireAbilityTaskDelegate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityGraphFlow(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Analysis
	static FMegalithActionResult HandleValidateAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindAbilitiesByTag(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityTagMatrix(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateAbilityBlueprint(const TSharedPtr<FJsonObject>& Params);
	// Phase 4: Advanced
	static FMegalithActionResult HandleScaffoldCustomAbilityTask(const TSharedPtr<FJsonObject>& Params);
};
