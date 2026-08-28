// SPDX-License-Identifier: MIT
// Plan: Plugins/Megalith/Docs/plans/2026-05-28-reflection-intelligence.md (Phase 1).

#include "MegalithReflectionIntelSettings.h"
#include "MegalithReflectionIntelModule.h"

#include "Modules/ModuleManager.h"
#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif

UMegalithReflectionIntelSettings::UMegalithReflectionIntelSettings()
{
	// Sensible Leviathan-corpus defaults. Indexer also supplies these as a
	// fallback when the array is empty, so authoring is optional.
	DecisionMarkdownRoots.Add(TEXT("Docs"));
	DecisionMarkdownRoots.Add(TEXT("Plugins/Megalith/Docs"));
	DecisionMarkdownRoots.Add(TEXT(".claude/rules"));

	// Phase 2 risk noise filter — default fragments that should be excluded
	// from co-change weighting. The fragments are substring-matched
	// case-insensitively against project-relative file paths.
	GitMiningNoiseFilter.Add(TEXT("CHANGELOG.md"));
	GitMiningNoiseFilter.Add(TEXT(".uplugin"));
	GitMiningNoiseFilter.Add(TEXT("Docs/plans/"));
	GitMiningNoiseFilter.Add(TEXT("Docs/testing/"));
}

const UMegalithReflectionIntelSettings* UMegalithReflectionIntelSettings::Get()
{
	return GetDefault<UMegalithReflectionIntelSettings>();
}

#if WITH_EDITOR
void UMegalithReflectionIntelSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Every Risk-category property either feeds the config fingerprint or
	// changes which repositories get mined, so all of them must re-arm the
	// bootstrap. GetMemberPropertyName is the array-safe accessor: editing an
	// element of GitRepoRoots / GitMiningNoiseFilter reports the ELEMENT under
	// GetPropertyName and the owning array only under GetMemberPropertyName.
	static const TArray<FName> RiskProperties = {
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, bEnableGitCoChangeMining),
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, GitRepoRoots),
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, bProbeAncestorsForGitRoot),
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, MaxCoChangeWindowCommits),
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, GitMiningNoiseFilter),
		GET_MEMBER_NAME_CHECKED(UMegalithReflectionIntelSettings, MaxCommitFileCount)
	};

	const FName Changed = PropertyChangedEvent.GetMemberPropertyName();
	if (Changed == NAME_None || !RiskProperties.Contains(Changed))
	{
		return;
	}

	if (FMegalithReflectionIntelModule* Module =
		FModuleManager::GetModulePtr<FMegalithReflectionIntelModule>(TEXT("MegalithReflectionIntel")))
	{
		Module->ClearRiskBootstrapAttempted();
	}
}
#endif
