// MegalithGASUIBindingActions.h
// Action handlers for the gas/ui attribute-binding feature (Phase H1).
// Canonical registration in `gas` namespace; thin alias in `ui` namespace.

#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASUIBindingActions
{
public:
    /** Registers actions in BOTH `gas` (canonical) and `ui` (alias) namespaces. */
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleBindWidgetToAttribute(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleUnbindWidgetAttribute(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleListAttributeBindings(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleClearWidgetAttributeBindings(const TSharedPtr<FJsonObject>& Params);
};
