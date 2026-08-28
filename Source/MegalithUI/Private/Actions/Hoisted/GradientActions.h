// Copyright tumourlove. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * MegalithUI -- parameter-driven gradient MID factory.
 *
 * create_gradient_mid_from_spec creates a UMaterialInstanceConstant from a
 * caller-supplied parent material. Zero hardcoded material paths. The parent
 * must expose:
 *   - scalar params  StopNPos        (N in [0,7])
 *   - vector params  StopNColor      (N in [0,7])
 *   - static switch  UseStopN        (N in [0,7])
 *   - scalar param   Angle           (optional -- written only if present)
 *
 * Validation short-circuits BEFORE asset creation: if the parent doesn't expose
 * Stop0Pos/Stop0Color the action returns -32602 without leaving a half-built
 * MIC on disk.
 */
namespace MegalithUI
{
    struct FGradientActions
    {
        static void Register(FMegalithToolRegistry& Registry);

        static FMegalithActionResult HandleCreateGradientMidFromSpec(const TSharedPtr<FJsonObject>& Params);
    };
}
