#pragma once

#include "MegalithIndexer.h"

/**
 * Indexes Materials and Material Instances: expression nodes,
 * connections, parameters (scalar, vector, texture).
 */
class FMaterialIndexer : public IMegalithIndexer
{
public:
	virtual TArray<FString> GetSupportedClasses() const override
	{
		return {
			TEXT("Material"),
			TEXT("MaterialInstanceConstant"),
			TEXT("MaterialFunction")
		};
	}

	virtual bool IndexAsset(const FAssetData& AssetData, UObject* LoadedAsset, FMegalithIndexDatabase& DB, int64 AssetId) override;
	virtual FString GetName() const override { return TEXT("MaterialIndexer"); }

private:
	void IndexMaterialExpressions(class UMaterial* Material, FMegalithIndexDatabase& DB, int64 AssetId);
	void IndexMaterialInstance(class UMaterialInstanceConstant* MIC, FMegalithIndexDatabase& DB, int64 AssetId);
};
