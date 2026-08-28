#include "Actions/ProjectGetAssetDetailsAction.h"
#include "MegalithIndexSubsystem.h"
#include "MegalithParamSchema.h"
#include "Editor.h"

FMegalithActionResult FProjectGetAssetDetailsAction::Execute(const TSharedPtr<FJsonObject>& Params)
{
	FString PackagePath = Params->GetStringField(TEXT("asset_path"));
	if (PackagePath.IsEmpty())
	{
		PackagePath = Params->GetStringField(TEXT("package_path"));
	}
	if (PackagePath.IsEmpty())
	{
		return FMegalithActionResult::Error(TEXT("'asset_path' (or 'package_path') parameter is required"), -32602);
	}

	UMegalithIndexSubsystem* Subsystem = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>();
	if (!Subsystem)
	{
		return FMegalithActionResult::Error(TEXT("Index subsystem not available"));
	}

	TSharedPtr<FJsonObject> Details = Subsystem->GetAssetDetails(PackagePath);
	if (!Details.IsValid() || !Details->HasField(TEXT("asset_name")))
	{
		return FMegalithActionResult::Error(TEXT("Asset not found in index"));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetObjectField(TEXT("asset"), Details);
	return FMegalithActionResult::Success(Result);
}

TSharedPtr<FJsonObject> FProjectGetAssetDetailsAction::GetSchema()
{
	return FParamSchemaBuilder()
		.Required(TEXT("asset_path"), TEXT("string"), TEXT("Package path of the asset (e.g. /Game/Characters/BP_Hero)"))
		.Build();
}
