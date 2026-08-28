#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintCompileActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleCompileBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDependencies(const TSharedPtr<FJsonObject>& Params);

	// Phase 1F — save_asset
	static FMegalithActionResult HandleSaveAsset(const TSharedPtr<FJsonObject>& Params);
	// Gap #10 — save all dirty Blueprint/Widget packages in one sweep
	static FMegalithActionResult HandleSaveDirtyAssets(const TSharedPtr<FJsonObject>& Params);
};
