#include "MegalithNiagaraModule.h"
#include "MegalithJsonUtils.h"
#include "MegalithNiagaraActions.h"
#include "MegalithNiagaraLayoutActions.h"
#include "MegalithNiagaraTimingActions.h"
#include "MegalithNiagaraBulkFillAdapter.h"
#include "MegalithToolRegistry.h"

#define LOCTEXT_NAMESPACE "FMegalithNiagaraModule"

void FMegalithNiagaraModule::StartupModule()
{
	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithNiagaraActions::RegisterActions(Registry);
	FMegalithNiagaraLayoutActions::RegisterActions(Registry);
	FMegalithNiagaraTimingActions::RegisterActions(Registry);

	// Phase 5 Step 2 (MCP Ergonomics, 2026-05-11) — register the niagara adapter.
	// No WITH_* gate (Niagara is a core engine plugin, always-on). Body rejects
	// GPU-sim params with a WISHLIST error and delegates per-field writes to the
	// FMegalithReflectionWalker.
	FMegalithNiagaraBulkFillAdapter::Register();

	UE_LOG(LogMegalith, Verbose, TEXT("Megalith — Niagara module loaded (42 actions)"));
}

void FMegalithNiagaraModule::ShutdownModule()
{
	FMegalithNiagaraBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("niagara"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithNiagaraModule, MegalithNiagara)
