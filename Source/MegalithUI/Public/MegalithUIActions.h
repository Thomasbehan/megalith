// MegalithUIActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleCreateWidgetBlueprint(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleGetWidgetTree(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleAddWidget(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleRemoveWidget(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetWidgetProperty(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCompileWidget(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleListWidgetTypes(const TSharedPtr<FJsonObject>& Params);
};
