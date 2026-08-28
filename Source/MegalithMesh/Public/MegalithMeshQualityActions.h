#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 22: Polish & Remaining (9 actions)
 * Quality-of-life, naming validation, proxy mesh generation, HLOD setup,
 * texture budget analysis, framing/composition scoring, monster reveal evaluation,
 * co-op balance placeholder, and integration hook stubs.
 */
class FMegalithMeshQualityActions
{
public:
	/** Register all 9 quality/polish actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- Naming & Organization ---
	static FMegalithActionResult ValidateNamingConventions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchRenameAssets(const TSharedPtr<FJsonObject>& Params);

	// --- Proxy & HLOD ---
	static FMegalithActionResult GenerateProxyMesh(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetupHlod(const TSharedPtr<FJsonObject>& Params);

	// --- Texture Budget ---
	static FMegalithActionResult AnalyzeTextureBudget(const TSharedPtr<FJsonObject>& Params);

	// --- Composition & Horror ---
	static FMegalithActionResult AnalyzeFraming(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult EvaluateMonsterReveal(const TSharedPtr<FJsonObject>& Params);

	// --- Co-op & Integration Stubs ---
	static FMegalithActionResult AnalyzeCoOpBalance(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult IntegrationHooksStub(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---
	static TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V);
};
