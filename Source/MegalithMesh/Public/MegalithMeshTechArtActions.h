#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class UMegalithMeshHandlePool;

/**
 * Phase 16: Tech Art Pipeline (7 actions)
 * Import, quality fix, LOD generation, texel density, material cost,
 * collision setup, and lightmap density analysis.
 */
class FMegalithMeshTechArtActions
{
public:
	/** Register all 7 tech art actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

#if WITH_GEOMETRYSCRIPT
	/** Set the handle pool instance (called during module startup) */
	static void SetHandlePool(UMegalithMeshHandlePool* InPool);
#endif

private:
	/** Import FBX/glTF mesh via IAssetTools::ImportAssetsAutomated */
	static FMegalithActionResult ImportMesh(const TSharedPtr<FJsonObject>& Params);

	/** Export a UStaticMesh / USkeletalMesh asset to FBX file on disk via UAssetExportTask */
	static FMegalithActionResult ExportMesh(const TSharedPtr<FJsonObject>& Params);

	/** Auto-fix mesh quality: weld, degenerate removal, hole fill, normals (GeometryScript) */
	static FMegalithActionResult FixMeshQuality(const TSharedPtr<FJsonObject>& Params);

	/** One-shot LOD generation: simplify + write back to UStaticMesh source models */
	static FMegalithActionResult AutoGenerateLods(const TSharedPtr<FJsonObject>& Params);

	/** Texel density analysis: UV area vs world-space area ratio per section */
	static FMegalithActionResult AnalyzeTexelDensity(const TSharedPtr<FJsonObject>& Params);

	/** Cross-module: spatial query + shader instruction count per material */
	static FMegalithActionResult AnalyzeMaterialCostInRegion(const TSharedPtr<FJsonObject>& Params);

	/** Set collision on a static mesh asset (simple shapes, complex, auto-convex) */
	static FMegalithActionResult SetMeshCollision(const TSharedPtr<FJsonObject>& Params);

	/** Lightmap texel density analysis and resolution recommendations */
	static FMegalithActionResult AnalyzeLightmapDensity(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---
	static TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V);

#if WITH_GEOMETRYSCRIPT
	static UMegalithMeshHandlePool* Pool;
#endif
};
