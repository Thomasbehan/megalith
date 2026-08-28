#pragma once

#include "MegalithIndexer.h"

/**
 * Indexes Blueprints: graphs, nodes, pins, connections, variables.
 * Walks every UEdGraph in the Blueprint, extracts node topology,
 * pin connections, and variable declarations.
 */
class FBlueprintIndexer : public IMegalithIndexer
{
public:
	virtual TArray<FString> GetSupportedClasses() const override
	{
		return { TEXT("Blueprint"), TEXT("WidgetBlueprint"), TEXT("AnimBlueprint") };
	}

	virtual bool IndexAsset(const FAssetData& AssetData, UObject* LoadedAsset, FMegalithIndexDatabase& DB, int64 AssetId) override;
	virtual FString GetName() const override { return TEXT("BlueprintIndexer"); }

private:
	void IndexGraph(class UEdGraph* Graph, FMegalithIndexDatabase& DB, int64 AssetId);
	void IndexVariables(class UBlueprint* Blueprint, FMegalithIndexDatabase& DB, int64 AssetId);
};
