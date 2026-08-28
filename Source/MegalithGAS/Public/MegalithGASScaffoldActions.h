#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASScaffoldActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 1: Scaffolding
	static FMegalithActionResult HandleBootstrapGASFoundation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateGASSetup(const TSharedPtr<FJsonObject>& Params);

	// Phase 2: Scaffolding
	static FMegalithActionResult HandleScaffoldGASProject(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldDamagePipeline(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldStatusEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldWeaponAbility(const TSharedPtr<FJsonObject>& Params);

	// Phase F8 (J-phase): author-time ability grant via ASC-CDO startup-array reflection
	static FMegalithActionResult HandleGrantAbilityToPawn(const TSharedPtr<FJsonObject>& Params);
};
