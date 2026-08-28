#include "Actions/ProjectGetStatsAction.h"
#include "MegalithIndexSubsystem.h"
#include "MegalithParamSchema.h"
#include "Editor.h"

FMegalithActionResult FProjectGetStatsAction::Execute(const TSharedPtr<FJsonObject>& Params)
{
	UMegalithIndexSubsystem* Subsystem = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>();
	if (!Subsystem)
	{
		return FMegalithActionResult::Error(TEXT("Index subsystem not available"));
	}

	TSharedPtr<FJsonObject> Stats = Subsystem->GetStats();
	if (!Stats.IsValid())
	{
		return FMegalithActionResult::Error(TEXT("Failed to retrieve stats"));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetBoolField(TEXT("success"), true);

	bool bIndexing = Subsystem->IsIndexing();
	Result->SetBoolField(TEXT("indexing"), bIndexing);
	if (bIndexing)
	{
		Result->SetNumberField(TEXT("progress"), Subsystem->GetProgress());
		Result->SetStringField(TEXT("status"), Subsystem->GetStatusMessage());
	}
	Result->SetObjectField(TEXT("stats"), Stats);
	return FMegalithActionResult::Success(Result);
}

TSharedPtr<FJsonObject> FProjectGetStatsAction::GetSchema()
{
	return MakeShared<FJsonObject>();
}
