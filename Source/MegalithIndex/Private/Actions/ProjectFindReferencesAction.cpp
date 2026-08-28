#include "Actions/ProjectFindReferencesAction.h"
#include "MegalithIndexSubsystem.h"
#include "MegalithParamSchema.h"
#include "Editor.h"

FMegalithActionResult FProjectFindReferencesAction::Execute(const TSharedPtr<FJsonObject>& Params)
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

	TSharedPtr<FJsonObject> Refs = Subsystem->FindReferences(PackagePath);
	if (!Refs.IsValid())
	{
		return FMegalithActionResult::Error(TEXT("Asset not found in index"));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);
	Result->SetStringField(TEXT("asset_path"), PackagePath);
	Result->SetObjectField(TEXT("references"), Refs);
	return FMegalithActionResult::Success(Result);
}

TSharedPtr<FJsonObject> FProjectFindReferencesAction::GetSchema()
{
	return FParamSchemaBuilder()
		.Required(TEXT("asset_path"), TEXT("string"), TEXT("Package path of the asset (e.g. /Game/Characters/BP_Hero)"))
		.Build();
}
