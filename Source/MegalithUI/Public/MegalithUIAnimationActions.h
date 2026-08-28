// MegalithUIAnimationActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIAnimationActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleListAnimations(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleGetAnimationDetails(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateAnimation(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleAddAnimationKeyframe(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleRemoveAnimation(const TSharedPtr<FJsonObject>& Params);
};
