#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASTargetActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 3: Targeting
	static FMegalithActionResult HandleCreateTargetActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureTargetActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddTargetingToAbility(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldFPSTargeting(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateTargeting(const TSharedPtr<FJsonObject>& Params);
};
