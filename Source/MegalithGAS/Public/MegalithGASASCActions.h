#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASASCActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: ASC Setup
	static FMegalithActionResult HandleAddASCToActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureASC(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetupASCInit(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetupAbilitySystemInterface(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Configuration
	static FMegalithActionResult HandleApplyASCTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDefaultAbilities(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDefaultEffects(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDefaultAttributeSets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetASCReplicationMode(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Validation
	static FMegalithActionResult HandleValidateASCSetup(const TSharedPtr<FJsonObject>& Params);
	// Phase 4: Runtime
	static FMegalithActionResult HandleGrantAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRevokeAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetASCSnapshot(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAllASCs(const TSharedPtr<FJsonObject>& Params);
};
