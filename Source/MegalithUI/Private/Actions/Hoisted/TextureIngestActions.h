// Copyright tumourlove. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * MegalithUI -- texture ingest action.
 *
 * Decodes a base64-encoded compressed image (PNG / JPEG / BMP / EXR / TGA /
 * HDR / TIFF / DDS) and imports it as a UTexture2D asset at a /Game/... path.
 * Mirrors the editor-import flow used elsewhere in MegalithUI: NewObject +
 * AssetRegistry::AssetCreated + SavePackage with CreateUniqueAssetName for
 * collision-safe naming.
 *
 * Editor-only -- FTextureSource is WITH_EDITOR-gated.
 */
namespace MegalithUI
{
    struct FTextureIngestActions
    {
        static void Register(FMegalithToolRegistry& Registry);

        static FMegalithActionResult HandleImportTextureFromBytes(const TSharedPtr<FJsonObject>& Params);
    };
}
