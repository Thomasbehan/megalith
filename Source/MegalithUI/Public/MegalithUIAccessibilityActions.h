// MegalithUIAccessibilityActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIAccessibilityActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleScaffoldAccessibilitySubsystem(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleAuditAccessibility(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetColorblindMode(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetTextScale(const TSharedPtr<FJsonObject>& Params);
};
