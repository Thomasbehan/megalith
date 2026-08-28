#include "MegalithConfigModule.h"
#include "MegalithConfigActions.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithSettings.h"

#define LOCTEXT_NAMESPACE "FMegalithConfigModule"

void FMegalithConfigModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableConfig) return;

	FMegalithConfigActions::RegisterActions(FMegalithToolRegistry::Get());
	UE_LOG(LogMegalith, Log, TEXT("Megalith — Config module loaded (6 actions)"));
}

void FMegalithConfigModule::ShutdownModule()
{
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("config"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithConfigModule, MegalithConfig)
