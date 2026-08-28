// Copyright Thomasbehan and Contributors. All Rights Reserved.
// MegalithUIRegistryActions.h
//
// MCP action registrations that surface the type registry / allowlist for
// diagnostic / discovery use. Lives separate from MegalithUIActions.h so the
// hot CRUD path (create_widget_blueprint, add_widget, set_widget_property)
// stays uncluttered as more registry-introspection actions land.

#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIRegistryActions
{
public:
    /** Bulk-register all registry diagnostic actions under the `ui` namespace. */
    static void RegisterActions(FMegalithToolRegistry& Registry);

    /** `ui::dump_property_allowlist` — returns `{type, allowed_paths:[...]}`. */
    static FMegalithActionResult HandleDumpPropertyAllowlist(const TSharedPtr<FJsonObject>& Params);
};
