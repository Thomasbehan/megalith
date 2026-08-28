#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASInputActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Input Binding
	static FMegalithActionResult HandleSetupAbilityInputBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBindAbilityToInput(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Input Binding Productivity
	static FMegalithActionResult HandleBatchBindAbilities(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbilityInputBindings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldInputBindingComponent(const TSharedPtr<FJsonObject>& Params);
};
