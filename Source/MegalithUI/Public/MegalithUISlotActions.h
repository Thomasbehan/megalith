// MegalithUISlotActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUISlotActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleSetSlotProperty(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetAnchorPreset(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleMoveWidget(const TSharedPtr<FJsonObject>& Params);
};
