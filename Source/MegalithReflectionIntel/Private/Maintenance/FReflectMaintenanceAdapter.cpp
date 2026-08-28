// SPDX-License-Identifier: MIT
// Plan: Plugins/Megalith/Docs/plans/2026-05-28-reflection-intelligence.md (Maintenance — v0.17.0).
//
// FReflectMaintenanceAdapter — implementation. A single write/maintenance
// handler that force-rebuilds the Reflection Intelligence reflect_* tables from
// PROJECT UHT artefacts only. See the header for the namespace-placement
// rationale and FMegalithReflectionIntelModule::ForceRebuildReflectionTables for
// the force-semantics + project-only contract.

#include "Maintenance/FReflectMaintenanceAdapter.h"
#include "MegalithReflectionIntelModule.h"
#include "MegalithRIMetaTable.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Modules/ModuleManager.h"
#include "MegalithParamSchema.h"

// ============================================================================
// Registration
// ============================================================================

void FReflectMaintenanceAdapter::RegisterActions(FMegalithToolRegistry& Registry)
{
	// ---- rebuild_reflection_index ----
	Registry.RegisterAction(TEXT("reflect"), TEXT("rebuild_reflection_index"),
		TEXT("MAINTENANCE (write): force-repopulate the Reflection Intelligence "
		     "reflect_* tables from PROJECT UHT artefacts only (engine excluded). "
		     "Rebuilds the cppreflect set (reflect_uclasses / reflect_uproperties / "
		     "reflect_ufunctions / reflect_uinterfaces / reflect_uinterface_impls "
		     "+ cpp_asset_edges) via FUHTArtefactReader + FAssetGraphJoiner, AND the "
		     "network set (reflect_replicated_properties) via FNetworkRepIndexer. "
		     "Each indexer's Run() WIPEs (DELETE FROM) its tables before "
		     "repopulating, so this is a genuine clear+rewrite — NOT an append. "
		     "Use this after the project is rebuilt with UBT (or after the indexer "
		     "parsing code changes) to refresh tables that would otherwise stay "
		     "stale: the lazy bootstrap only fires when a table is ABSENT, and "
		     "source.trigger_(project_)reindex rebuilds the source-symbol tables, "
		     "NOT these reflect_* tables. Project-scoped: roots resolve to "
		     "Intermediate/Build and bIncludeEnginePlugins follows the "
		     "MegalithReflectionIntel setting (default false) — it never scans the "
		     "engine or rebuilds the engine source-symbol index. Requires the "
		     "project to have been built with UBT at least once (Live Coding "
		     "patches do not emit gen.cpp). Returns a per-indexer status object. "
		     "Game-thread; no parameters."),
		FMegalithActionHandler::CreateStatic(&FReflectMaintenanceAdapter::HandleRebuildReflectionIndex),
		FParamSchemaBuilder().Build());

	// Dispatcher annotation — this dispatcher hosts a WRITE/maintenance action,
	// so it is NOT read-only. It is non-destructive (idempotent regeneration: the
	// tables are wiped + rebuilt from on-disk artefacts, never partially mutated
	// from caller input) and idempotent (re-running yields identical tables for
	// unchanged artefacts). readOnlyHint is deliberately left false to signal the
	// index mutation to clients.
	FMegalithDispatcherAnnotations Anno;
	Anno.bReadOnlyHint    = false;
	Anno.bDestructiveHint = false;
	Anno.bIdempotentHint  = true;
	Anno.Title = TEXT("Reflection-index maintenance");
	Registry.SetDispatcherAnnotations(TEXT("reflect"), Anno);
}

// ============================================================================
// Handler
// ============================================================================

FMegalithActionResult FReflectMaintenanceAdapter::HandleRebuildReflectionIndex(
	const TSharedPtr<FJsonObject>& /*Params*/)
{
	// Game-thread only — ForceRebuildReflectionTables touches GEditor + the shared
	// SQLite handle. Assert here at the dispatch boundary (the module method and
	// the indexers' Run() also assert, but failing fast at entry is clearer).
	ensure(IsInGameThread());

	FMegalithReflectionIntelModule* Module =
		FModuleManager::GetModulePtr<FMegalithReflectionIntelModule>(
			TEXT("MegalithReflectionIntel"));
	if (!Module)
	{
		return FMegalithActionResult::Error(
			TEXT("MegalithReflectionIntel module not loaded."));
	}

	FString CppReflectStatus;
	FString NetworkStatus;
	const bool bAllOk = Module->ForceRebuildReflectionTables(CppReflectStatus, NetworkStatus);

	// Per-indexer status object. `rebuilt` is the AND of both runner results so a
	// caller can branch on overall success while still reading each indexer's
	// detailed status string (which carries row counts, "no UHT artefacts found"
	// guidance, or the "source DB not open — bootstrap" message on failure).
	TSharedPtr<FJsonObject> Out = MakeShared<FJsonObject>();
	Out->SetBoolField(TEXT("rebuilt"), bAllOk);
	Out->SetStringField(TEXT("cppreflect"), CppReflectStatus);
	Out->SetStringField(TEXT("network"), NetworkStatus);
	Out->SetBoolField(TEXT("project_only"), true);

	// Handover doc item #1 — surface the stamped indexer-code-version for each
	// RI subsystem so callers can verify the freshly-rebuilt tables carry the
	// current compiled version (and bump-out-of-band detection has somewhere to
	// land in the response). All four subsystems' constants live in
	// MegalithRIMetaTable.cpp; we just project them.
	{
		TSharedPtr<FJsonObject> Versions = MakeShared<FJsonObject>();
		Versions->SetNumberField(TEXT("cppreflect"),
			MegalithRIMeta::GetIndexerCodeVersion(TEXT("cppreflect")));
		Versions->SetNumberField(TEXT("network"),
			MegalithRIMeta::GetIndexerCodeVersion(TEXT("network")));
		Versions->SetNumberField(TEXT("risk"),
			MegalithRIMeta::GetIndexerCodeVersion(TEXT("risk")));
		Versions->SetNumberField(TEXT("decision"),
			MegalithRIMeta::GetIndexerCodeVersion(TEXT("decision")));
		Out->SetObjectField(TEXT("meta_versions"), Versions);
	}

	// Handover doc item #2 — surface per-subsystem scanned/skipped roots. The
	// indexers track these (FUHTArtefactReader / FNetworkRepIndexer optional
	// output params); the module's runner stamps the latest sets onto its
	// instance. Project them into the response so silent "root added but it
	// doesn't exist" failures are visible (vs the previous Verbose-only log).
	auto PathsToJson = [](const TArray<FString>& Paths)
	{
		TArray<TSharedPtr<FJsonValue>> A;
		A.Reserve(Paths.Num());
		for (const FString& P : Paths) { A.Add(MakeShared<FJsonValueString>(P)); }
		return A;
	};
	auto SkipsToJson = [](const TArray<TPair<FString, FString>>& Skipped)
	{
		TArray<TSharedPtr<FJsonValue>> A;
		A.Reserve(Skipped.Num());
		for (const TPair<FString, FString>& S : Skipped)
		{
			TSharedPtr<FJsonObject> O = MakeShared<FJsonObject>();
			O->SetStringField(TEXT("path"),   S.Key);
			O->SetStringField(TEXT("reason"), S.Value);
			A.Add(MakeShared<FJsonValueObject>(O));
		}
		return A;
	};
	{
		TSharedPtr<FJsonObject> CppDiag = MakeShared<FJsonObject>();
		CppDiag->SetArrayField(TEXT("roots_scanned"),
			PathsToJson(Module->GetLastCppReflectScannedRoots()));
		CppDiag->SetArrayField(TEXT("roots_skipped"),
			SkipsToJson(Module->GetLastCppReflectSkippedRoots()));
		Out->SetObjectField(TEXT("cppreflect_diagnostics"), CppDiag);

		TSharedPtr<FJsonObject> NetDiag = MakeShared<FJsonObject>();
		NetDiag->SetArrayField(TEXT("roots_scanned"),
			PathsToJson(Module->GetLastNetworkScannedRoots()));
		NetDiag->SetArrayField(TEXT("roots_skipped"),
			SkipsToJson(Module->GetLastNetworkSkippedRoots()));
		Out->SetObjectField(TEXT("network_diagnostics"), NetDiag);
	}
	return FMegalithActionResult::Success(Out);
}
