#if WITH_GEOMETRYSCRIPT

#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class UMegalithMeshHandlePool;

/**
 * Phase 5: Mesh Operation Actions (12 actions, GeometryScript)
 * Handle pool-based mesh editing: create, release, list, save handles,
 * boolean, simplify, remesh, collision, LODs, fill holes, UVs, mirror.
 */
class FMegalithMeshOperationActions
{
public:
	/** Register all 12 operation actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

	/** Set the handle pool instance (called during module startup) */
	static void SetHandlePool(UMegalithMeshHandlePool* InPool);

private:
	static UMegalithMeshHandlePool* Pool;

	// Handle management
	static FMegalithActionResult CreateHandle(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ReleaseHandle(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListHandles(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SaveHandle(const TSharedPtr<FJsonObject>& Params);

	// Mesh operations
	static FMegalithActionResult MeshBoolean(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult MeshSimplify(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult MeshRemesh(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GenerateCollision(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GenerateLods(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FillHoles(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ComputeUvs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult MirrorMesh(const TSharedPtr<FJsonObject>& Params);
};

#endif // WITH_GEOMETRYSCRIPT
