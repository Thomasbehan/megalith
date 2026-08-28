#include "MegalithGASModule.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithGASAbilityActions.h"
#include "MegalithGASAttributeActions.h"
#include "MegalithGASEffectActions.h"
#include "MegalithGASASCActions.h"
#include "MegalithGASTagActions.h"
#include "MegalithGASCueActions.h"
#include "MegalithGASTargetActions.h"
#include "MegalithGASInputActions.h"
#include "MegalithGASInspectActions.h"
#include "MegalithGASScaffoldActions.h"
#include "MegalithGASUIBindingActions.h"
#include "MegalithGASBulkFillAdapter.h"

DEFINE_LOG_CATEGORY(LogMegalithGAS);

void FMegalithGASModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings || !Settings->bEnableGAS)
	{
		UE_LOG(LogMegalithGAS, Log,
			TEXT("MegalithGAS: GAS integration disabled in settings"));
		return;
	}

	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithGASAbilityActions::RegisterActions(Registry);
	FMegalithGASAttributeActions::RegisterActions(Registry);
	FMegalithGASEffectActions::RegisterActions(Registry);
	FMegalithGASASCActions::RegisterActions(Registry);
	FMegalithGASTagActions::RegisterActions(Registry);
	FMegalithGASCueActions::RegisterActions(Registry);
	FMegalithGASTargetActions::RegisterActions(Registry);
	FMegalithGASInputActions::RegisterActions(Registry);
	FMegalithGASInspectActions::RegisterActions(Registry);
	FMegalithGASScaffoldActions::RegisterActions(Registry);
	FMegalithGASUIBindingActions::RegisterActions(Registry);

	// Phase 2 (MCP Ergonomics) — register the gas adapter on the central
	// FMegalithBulkFillRegistry. The Register() call ALWAYS runs (H5 invariant)
	// regardless of WITH_GBA so `megalith_discover("gas")` action surface stays
	// identical across dev + release builds; the adapter BODY switches on
	// WITH_GBA and returns a clean "GAS not available" error when the optional
	// dep is absent. See MegalithGASBulkFillAdapter.cpp for the split.
	FMegalithGASBulkFillAdapter::Register();

	int32 ActionCount = Registry.GetActions(TEXT("gas")).Num();
	const TCHAR* GbaStatus =
#if WITH_GBA
		TEXT("available");
#else
		TEXT("not installed");
#endif
	UE_LOG(LogMegalithGAS, Log, TEXT("MegalithGAS: Loaded (%d actions, GBA=%s)"), ActionCount, GbaStatus);
}

void FMegalithGASModule::ShutdownModule()
{
	FMegalithGASBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("gas"));
}

IMPLEMENT_MODULE(FMegalithGASModule, MegalithGAS)
