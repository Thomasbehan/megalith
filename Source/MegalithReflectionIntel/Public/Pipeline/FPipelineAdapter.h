// SPDX-License-Identifier: MIT
// Plan: Plugins/Megalith/Docs/plans/2026-05-28-reflection-intelligence.md (Phase 4a — v0.17.0).
//
// FPipelineAdapter — registers the `pipeline` namespace (2 composer actions)
// against the central FMegalithToolRegistry. Pure composition over existing
// actions; this adapter holds NO state of its own.
//
// Phase 4a composer surface:
//   pipeline_query("pr_review", {changed_files=[...]})
//   pipeline_query("release_readiness", {target_release?})
//
// Both composers fan out to several namespaces' actions and return a single
// envelope object with one section per check. Dispatcher annotated
// readOnlyHint=true + idempotentHint=true — composers do nothing destructive.

#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class MEGALITHREFLECTIONINTEL_API FPipelineAdapter
{
public:
	/** Register `pipeline_query("pr_review")` + `pipeline_query("release_readiness")`
	 *  + dispatcher annotations. */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandlePRReview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReleaseReadiness(const TSharedPtr<FJsonObject>& Params);
};
