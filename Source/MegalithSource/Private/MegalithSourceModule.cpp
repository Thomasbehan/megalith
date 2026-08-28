#include "MegalithSourceModule.h"
#include "MegalithSourceActions.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithJsonUtils.h"

#define LOCTEXT_NAMESPACE "FMegalithSourceModule"

void FMegalithSourceModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableSource) return;

	FMegalithSourceActions::RegisterAll();
	UE_LOG(LogMegalith, Log, TEXT("Megalith — Source module loaded (10 actions)"));
}

void FMegalithSourceModule::ShutdownModule()
{
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("source"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithSourceModule, MegalithSource)
