// SPDX-License-Identifier: MIT
// Plan: Plugins/Megalith/Docs/plans/2026-05-28-reflection-intelligence.md (Phase 1).
//
// FDecisionQueryAdapter — registers the `decision_query` namespace (5 actions)
// against the central FMegalithToolRegistry. Pure read-only handlers; no
// indexing happens here (that's FDecisionRecordIndexer's job).
//
// v0.17.0 ergonomics adoption:
//   - `path_filter` params tagged EMegalithParamKind::DiskPath (auto `\`→`/`).
//   - Dispatcher annotated readOnlyHint=true via SetDispatcherAnnotations.
//   - `list_decisions` adopts cursor pagination via MegalithCursorCodec.
//   - Response shaping (_fields/_omit/_compact_json) participates automatically
//     via the registry's dispatch path — no per-action shaping code.
//   - Fuzzy-match suggestions on unknown actions emerge for free.

#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class MEGALITHREFLECTIONINTEL_API FDecisionQueryAdapter
{
public:
	/** Register all 5 decision_query actions + dispatcher annotations. */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Action handlers ---
	static FMegalithActionResult HandleListDecisions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDecision(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListStale(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindSupersessionChain(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindReferentDecisions(const TSharedPtr<FJsonObject>& Params);

	// --- Shared DB accessor ---
	static class FSQLiteDatabase* GetRawDB();
};
