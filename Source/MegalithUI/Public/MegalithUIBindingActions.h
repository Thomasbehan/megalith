// MegalithUIBindingActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIBindingActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleListWidgetEvents(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleListWidgetProperties(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetupListView(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleGetWidgetBindings(const TSharedPtr<FJsonObject>& Params);
};
