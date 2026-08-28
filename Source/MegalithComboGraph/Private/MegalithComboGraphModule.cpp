#include "MegalithComboGraphModule.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithComboGraphActions.h"
#include "MegalithComboGraphBulkFillAdapter.h"

DEFINE_LOG_CATEGORY(LogMegalithComboGraph);

void FMegalithComboGraphModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings || !Settings->bEnableComboGraph)
	{
		UE_LOG(LogMegalithComboGraph, Log,
			TEXT("MegalithComboGraph: ComboGraph integration disabled in settings"));
		return;
	}

#if WITH_COMBOGRAPH
	FMegalithComboGraphActions::RegisterActions(FMegalithToolRegistry::Get());
	int32 ActionCount = FMegalithToolRegistry::Get().GetActions(TEXT("combograph")).Num();
	UE_LOG(LogMegalithComboGraph, Log,
		TEXT("MegalithComboGraph: Loaded (%d actions)"), ActionCount);
#else
	UE_LOG(LogMegalithComboGraph, Log,
		TEXT("MegalithComboGraph: ComboGraph plugin not found at compile time, bridge inactive"));
#endif

	// Phase 5 Step 8 (MCP Ergonomics, 2026-05-11) — register the combograph adapter
	// UNCONDITIONALLY per H5 stub-adapter invariant. Body switches on WITH_COMBOGRAPH:
	// dev build wires real handlers, release/no-ComboGraph build returns a clean
	// error. **TargetType writes return EXPLICIT unsupported-field error** pointing
	// at the v1.1 custom-serialisation hook — NOT a silent no-op (Step 8 post-review
	// lock).
	FMegalithComboGraphBulkFillAdapter::Register();
}

void FMegalithComboGraphModule::ShutdownModule()
{
	FMegalithComboGraphBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("combograph"));
}

IMPLEMENT_MODULE(FMegalithComboGraphModule, MegalithComboGraph)
