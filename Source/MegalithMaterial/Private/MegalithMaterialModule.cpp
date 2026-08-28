#include "MegalithMaterialModule.h"
#include "MegalithMaterialActions.h"
#include "MegalithMaterialBulkFillAdapter.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithSettings.h"

#define LOCTEXT_NAMESPACE "FMegalithMaterialModule"

void FMegalithMaterialModule::StartupModule()
{
	if (!GetDefault<UMegalithSettings>()->bEnableMaterial) return;

	FMegalithMaterialActions::RegisterActions(FMegalithToolRegistry::Get());

	// Phase 5 Step 3 (MCP Ergonomics, 2026-05-11) — register the material adapter.
	// No WITH_* gate (material editor always-on). Body uses MIC's typed setters
	// (SetScalarParameterValueEditorOnly, etc.) for MICParameters fill_kind and
	// audits BuildMaterialGraph for SilentDrops; rejects MaterialAttributeLayers
	// writes per design Non-Goals §29.
	FMegalithMaterialBulkFillAdapter::Register();

	UE_LOG(LogMegalith, Log, TEXT("Megalith — Material module loaded (25 actions)"));
}

void FMegalithMaterialModule::ShutdownModule()
{
	FMegalithMaterialBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("material"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithMaterialModule, MegalithMaterial)
