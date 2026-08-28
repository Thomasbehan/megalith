// Copyright tumourlove. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * MegalithUI -- reflection-based rounded-corner property writer.
 *
 * Writes CornerRadii / OutlineColor / OutlineWidth / FillColor UPROPERTY fields
 * on a named widget inside a Widget Blueprint. Pure reflection via FProperty --
 * zero compile-time dependency on any specific widget class. Works on any widget
 * that exposes UPROPERTY fields with compatible names + types.
 *
 * Editor-only -- UBaseWidgetBlueprint::WidgetTree is WITH_EDITORONLY_DATA.
 */
namespace MegalithUI
{
    struct FRoundedCornerActions
    {
        static void Register(FMegalithToolRegistry& Registry);

        static FMegalithActionResult HandleSetRoundedCorners(const TSharedPtr<FJsonObject>& Params);
    };
}
