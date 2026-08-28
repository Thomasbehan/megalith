#include "MegalithLogicDriverModule.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithLogicDriverAssetActions.h"
#include "MegalithLogicDriverGraphActions.h"
#include "MegalithLogicDriverNodeActions.h"
#include "MegalithLogicDriverRuntimeActions.h"
#include "MegalithLogicDriverSpecActions.h"
#include "MegalithLogicDriverScaffoldActions.h"
#include "MegalithLogicDriverDiscoveryActions.h"
#include "MegalithLogicDriverComponentActions.h"
#include "MegalithLogicDriverTextGraphActions.h"
#include "MegalithLogicDriverBulkFillAdapter.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMegalithLogicDriver, Log, All);
DEFINE_LOG_CATEGORY(LogMegalithLogicDriver);

void FMegalithLogicDriverModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings || !Settings->bEnableLogicDriver)
	{
		UE_LOG(LogMegalithLogicDriver, Log,
			TEXT("MegalithLogicDriver: LogicDriver integration disabled in settings"));
		return;
	}

#if WITH_LOGICDRIVER
	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithLogicDriverAssetActions::RegisterActions(Registry);
	FMegalithLogicDriverGraphActions::RegisterActions(Registry);
	FMegalithLogicDriverNodeActions::RegisterActions(Registry);
	FMegalithLogicDriverRuntimeActions::RegisterActions(Registry);
	FMegalithLogicDriverSpecActions::RegisterActions(Registry);
	FMegalithLogicDriverScaffoldActions::RegisterActions(Registry);
	FMegalithLogicDriverDiscoveryActions::RegisterActions(Registry);
	FMegalithLogicDriverComponentActions::RegisterActions(Registry);
	FMegalithLogicDriverTextGraphActions::RegisterActions(Registry);
	int32 ActionCount = Registry.GetActions(TEXT("logicdriver")).Num();
	UE_LOG(LogMegalithLogicDriver, Log,
		TEXT("MegalithLogicDriver: Loaded (%d actions)"), ActionCount);
#else
	UE_LOG(LogMegalithLogicDriver, Log,
		TEXT("MegalithLogicDriver: Logic Driver Pro not found at compile time, bridge inactive"));
#endif

	// Phase 5 Step 7 (MCP Ergonomics, 2026-05-11) — register the logicdriver adapter
	// UNCONDITIONALLY per H5 stub-adapter invariant. Body switches on WITH_LOGICDRIVER:
	// dev build wires real handlers, release/no-LogicDriver build returns a clean
	// error so `megalith_discover("logicdriver")` action surface stays identical
	// across dev + release builds.
	FMegalithLogicDriverBulkFillAdapter::Register();
}

void FMegalithLogicDriverModule::ShutdownModule()
{
	FMegalithLogicDriverBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("logicdriver"));
}

IMPLEMENT_MODULE(FMegalithLogicDriverModule, MegalithLogicDriver)
