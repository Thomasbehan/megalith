#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintStructActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleCreateUserDefinedStruct(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateUserDefinedEnum(const TSharedPtr<FJsonObject>& Params);

	// DataTable actions (Phase 3C)
	static FMegalithActionResult HandleCreateDataTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddDataTableRow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDataTableRows(const TSharedPtr<FJsonObject>& Params);

	// Raw UObject asset creation (not Blueprint)
	static FMegalithActionResult HandleCreateDataAsset(const TSharedPtr<FJsonObject>& Params);

	// Create + populate a DataAsset in one call (create_data_asset body + reflection-walker fill).
	static FMegalithActionResult HandleSeedDataAsset(const TSharedPtr<FJsonObject>& Params);
};
