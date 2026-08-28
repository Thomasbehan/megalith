#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

#if WITH_GEOMETRYSCRIPT

class UMegalithMeshHandlePool;
class UDynamicMesh;

/**
 * SP4: Roof Generation
 * Generates roof geometry from building footprint polygons.
 * Supports: gable, hip, flat/parapet, shed, gambrel.
 * Consumes FootprintPolygon from the Building Descriptor (SP1).
 */
class FMegalithMeshRoofActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);
	static void SetHandlePool(UMegalithMeshHandlePool* InPool);

private:
	static UMegalithMeshHandlePool* Pool;

	// Action handler
	static FMegalithActionResult GenerateRoof(const TSharedPtr<FJsonObject>& Params);

	// ---- Roof type builders ----
	// Each appends geometry to TargetMesh. Returns false + OutError on failure.
	// HeightOffset = Z where the roof base sits (top of building walls).
	// OverhangPoly = footprint expanded outward by overhang distance.

	static bool BuildGableRoof(UDynamicMesh* Mesh, const TArray<FVector2D>& FootprintPoly,
		const TArray<FVector2D>& OverhangPoly, float PitchDeg, float HeightOffset,
		float Overhang, FString& OutError);

	static bool BuildHipRoof(UDynamicMesh* Mesh, const TArray<FVector2D>& FootprintPoly,
		const TArray<FVector2D>& OverhangPoly, float PitchDeg, float HeightOffset,
		float Overhang, FString& OutError);

	static bool BuildFlatRoof(UDynamicMesh* Mesh, const TArray<FVector2D>& FootprintPoly,
		float HeightOffset, float ParapetHeight, float ParapetThickness, FString& OutError);

	static bool BuildShedRoof(UDynamicMesh* Mesh, const TArray<FVector2D>& FootprintPoly,
		const TArray<FVector2D>& OverhangPoly, float PitchDeg, float HeightOffset,
		float Overhang, float RidgeOffset, FString& OutError);

	static bool BuildGambrelRoof(UDynamicMesh* Mesh, const TArray<FVector2D>& FootprintPoly,
		const TArray<FVector2D>& OverhangPoly, float PitchDeg, float HeightOffset,
		float Overhang, FString& OutError);

	// ---- Helpers ----

	/** Compute oriented bounding box of a 2D polygon. Returns center, half-extents, and the long-axis direction. */
	static void ComputeFootprintOBB(const TArray<FVector2D>& Polygon,
		FVector2D& OutCenter, float& OutHalfW, float& OutHalfD,
		FVector2D& OutLongAxis, FVector2D& OutShortAxis);

	/** Expand a polygon outward by a fixed distance (negative inset). */
	static TArray<FVector2D> ExpandPolygon(const TArray<FVector2D>& Polygon, float Distance);

	/** Parse footprint_polygon from JSON array of [x,y] pairs. */
	static bool ParseFootprint(const TSharedPtr<FJsonObject>& Params, TArray<FVector2D>& OutPoly, FString& OutError);
};

#endif // WITH_GEOMETRYSCRIPT
