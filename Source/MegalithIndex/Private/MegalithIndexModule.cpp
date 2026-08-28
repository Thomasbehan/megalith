#include "MegalithIndexModule.h"
#include "MegalithIndexDatabase.h"
#include "MegalithToolRegistry.h"
#include "Actions/ProjectSearchAction.h"
#include "Actions/ProjectFindReferencesAction.h"
#include "Actions/ProjectFindByTypeAction.h"
#include "Actions/ProjectGetStatsAction.h"
#include "Actions/ProjectGetAssetDetailsAction.h"
#include "Actions/ProjectListGameplayTagsAction.h"
#include "Actions/ProjectSearchGameplayTagsAction.h"
#include "Actions/ProjectRefreshAssetsAction.h"
#include "Actions/ProjectGetSavedAssetStateAction.h"
#include "Actions/ProjectCleanupGeneratedAssetsAction.h"
#include "Actions/ProjectExportAssetTextAction.h"

#define LOCTEXT_NAMESPACE "FMegalithIndexModule"

void FMegalithIndexModule::StartupModule()
{
	UE_LOG(LogMegalithIndex, Verbose, TEXT("Megalith -- Index module loaded (10 actions, SQLite+FTS5)"));

	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();

	Registry.RegisterAction(TEXT("project"), FProjectSearchAction::GetName(),
		FProjectSearchAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectSearchAction::Execute),
		FProjectSearchAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectFindReferencesAction::GetName(),
		FProjectFindReferencesAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectFindReferencesAction::Execute),
		FProjectFindReferencesAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectFindByTypeAction::GetName(),
		FProjectFindByTypeAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectFindByTypeAction::Execute),
		FProjectFindByTypeAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectGetStatsAction::GetName(),
		FProjectGetStatsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectGetStatsAction::Execute),
		FProjectGetStatsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectGetAssetDetailsAction::GetName(),
		FProjectGetAssetDetailsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectGetAssetDetailsAction::Execute),
		FProjectGetAssetDetailsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectListGameplayTagsAction::GetName(),
		FProjectListGameplayTagsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectListGameplayTagsAction::Execute),
		FProjectListGameplayTagsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectSearchGameplayTagsAction::GetName(),
		FProjectSearchGameplayTagsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectSearchGameplayTagsAction::Execute),
		FProjectSearchGameplayTagsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectRefreshAssetsAction::GetName(),
		FProjectRefreshAssetsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectRefreshAssetsAction::Execute),
		FProjectRefreshAssetsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectGetSavedAssetStateAction::GetName(),
		FProjectGetSavedAssetStateAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectGetSavedAssetStateAction::Execute),
		FProjectGetSavedAssetStateAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectCleanupGeneratedAssetsAction::GetName(),
		FProjectCleanupGeneratedAssetsAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectCleanupGeneratedAssetsAction::Execute),
		FProjectCleanupGeneratedAssetsAction::GetSchema());

	Registry.RegisterAction(TEXT("project"), FProjectExportAssetTextAction::GetName(),
		FProjectExportAssetTextAction::GetDescription(),
		FMegalithActionHandler::CreateStatic(&FProjectExportAssetTextAction::Execute),
		FProjectExportAssetTextAction::GetSchema());
}

void FMegalithIndexModule::ShutdownModule()
{
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("project"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithIndexModule, MegalithIndex)
