#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Editorial cross-namespace workflow guide for AI agents — primary audience is
 * EXTERNAL public-Megalith users with no project CLAUDE.md routing or private skills.
 * Hybrid: hand-authored markdown at Plugins/Megalith/Docs/MEGALITH_GUIDE.md
 * + live registry overlay (action counts, gate status, plugin version).
 * Registered under the "megalith" namespace as action "guide".
 */
class FMegalithGuideTool
{
public:
	/** Register the guide action (called from FMegalithCoreTools::RegisterAll). */
	static void RegisterAll();

	/** megalith.guide — return the editorial guide, optionally filtered to a named H2 section. */
	static FMegalithActionResult HandleGuide(const TSharedPtr<FJsonObject>& Params);

private:
	/** Load MEGALITH_GUIDE.md from the plugin Docs/ dir. Cached after first successful load. */
	static bool LoadGuideMarkdown(FString& OutMarkdown, FString& OutErrorMessage);

	/** Split markdown into named sections keyed by H2 header ("## <name>"). */
	static void SplitSections(const FString& Markdown, TMap<FString, FString>& OutSections, TArray<FString>& OutOrderedNames);

	/** Live overlay JSON: per-namespace action counts + gate status + plugin version. */
	static TSharedPtr<FJsonObject> BuildLiveOverlay();
};
