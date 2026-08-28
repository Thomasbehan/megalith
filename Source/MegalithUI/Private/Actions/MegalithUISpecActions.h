// Copyright Thomasbehan and Contributors. All Rights Reserved.
// MegalithUISpecActions.h
//
// Phase H — entry point for `ui::build_ui_from_spec` + `ui::dump_ui_spec_schema`.
// Implementation in the matching .cpp lives entirely under the
// `MegalithUI::SpecActionsInternal` anonymous-style namespace; only the
// register hook below is exposed.

#pragma once

#include "CoreMinimal.h"

class FMegalithToolRegistry;

namespace MegalithUI
{
    struct FSpecActions
    {
        /** Bulk-register `build_ui_from_spec` + `dump_ui_spec_schema` under the `ui` namespace. */
        static void Register(FMegalithToolRegistry& Registry);
    };
}
