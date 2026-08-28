#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 1: Mesh Inspection Actions (12 read-only actions)
 * Provides mesh info, bounds, materials, LODs, collision, UVs,
 * quality analysis, comparison, vertex data, and catalog queries.
 */
class FMegalithMeshInspectionActions
{
public:
	/** Register all 12 inspection actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Individual mesh inspection ---
	static FMegalithActionResult GetMeshInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshBounds(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshMaterials(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshLods(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshCollision(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshUvs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeSkeletalMesh(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeMeshQuality(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CompareMeshes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetVertexData(const TSharedPtr<FJsonObject>& Params);

	// --- Catalog queries (require MeshCatalogIndexer to have run) ---
	static FMegalithActionResult SearchMeshesBySize(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMeshCatalogStats(const TSharedPtr<FJsonObject>& Params);
};
