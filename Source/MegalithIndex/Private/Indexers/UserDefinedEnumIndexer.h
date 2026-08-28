#pragma once

#include "MegalithIndexer.h"

class FUserDefinedEnumIndexer : public IMegalithIndexer
{
public:
	virtual TArray<FString> GetSupportedClasses() const override
	{
		return { TEXT("UserDefinedEnum") };
	}

	virtual bool IndexAsset(const FAssetData& AssetData, UObject* LoadedAsset, FMegalithIndexDatabase& DB, int64 AssetId) override;
	virtual FString GetName() const override { return TEXT("UserDefinedEnumIndexer"); }
};
