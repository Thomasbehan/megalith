#pragma once

#include "CoreMinimal.h"

#if WITH_METASOUND

class FMegalithToolRegistry;
struct FMegalithActionResult;

/**
 * MetaSound document-introspection action pack (read-only).
 *
 * Ported from PR #18 by @alakangas. Walks on-disk MetaSound asset state via
 * IMetaSoundDocumentInterface::GetConstDocument(), returning structured JSON for
 * graph pages, nodes, edges, variables, user-parameters, dependencies, and
 * validation diagnostics. NO mutation of asset state — pure read side.
 *
 * Distinct from FMegalithAudioMetaSoundActions (Builder API write-side, 25 actions);
 * both register into the same `audio` namespace. See plan
 * Docs/plans/2026-05-03-metasound-indexer-integration.md § 4 (Q1) for action-name
 * disambiguation rationale.
 */
class FMegalithAudioMetaSoundIntrospectionActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// 12 read-only document-introspection handlers (Phase 3 of the integration plan).
	static FMegalithActionResult HandleListMetaSounds(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListMetaSoundDocuments(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundDocument(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundSummary(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleInspectMetaSoundNodeInstance(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundDocumentConnections(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundDocumentVariables(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundUserParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchMetaSoundDocumentNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMetaSoundDependencies(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateMetaSound(const TSharedPtr<FJsonObject>& Params);
};

#endif // WITH_METASOUND
