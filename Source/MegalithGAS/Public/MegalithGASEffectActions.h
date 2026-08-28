#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASEffectActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Core CRUD
	static FMegalithActionResult HandleCreateGameplayEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetGameplayEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListGameplayEffects(const TSharedPtr<FJsonObject>& Params);
	// Modifiers
	static FMegalithActionResult HandleAddModifier(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetModifier(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveModifier(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListModifiers(const TSharedPtr<FJsonObject>& Params);
	// Components & Configuration
	static FMegalithActionResult HandleAddGEComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetGEComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEffectStacking(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDuration(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetPeriod(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Productivity
	static FMegalithActionResult HandleCreateEffectFromTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildEffectFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchCreateEffects(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddExecution(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateGameplayEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteGameplayEffect(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Analysis
	static FMegalithActionResult HandleValidateEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEffectInteractionMatrix(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveGEComponent(const TSharedPtr<FJsonObject>& Params);
	// Phase 4: Runtime
	static FMegalithActionResult HandleGetActiveEffects(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEffectModifiersBreakdown(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleApplyEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSimulateEffectStack(const TSharedPtr<FJsonObject>& Params);
};
