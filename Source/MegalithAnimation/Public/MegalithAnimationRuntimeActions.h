#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * PIE-runtime animation telemetry actions for Megalith.
 *
 * Modeled on FMegalithLogicDriverRuntimeActions — resolves a live PIE actor's
 * USkeletalMeshComponent + UAnimInstance and reports live state (active state
 * machine state, montage, requested anim-instance variables, bone/socket
 * transforms). Read-only sampling; no graph mutation.
 */
class FMegalithAnimationRuntimeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleSamplePIEAnimInstance(const TSharedPtr<FJsonObject>& Params);
};
