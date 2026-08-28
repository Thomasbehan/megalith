#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Core discovery and status tool implementations.
 * These are registered under the "megalith" namespace.
 */
class FMegalithCoreTools
{
public:
	/** Register all core tools with the registry */
	static void RegisterAll();

	// --- Action Handlers ---

	/** megalith_discover — List available namespaces and their actions */
	static FMegalithActionResult HandleDiscover(const TSharedPtr<FJsonObject>& Params);

	/** megalith_status — Server health, version, index status */
	static FMegalithActionResult HandleStatus(const TSharedPtr<FJsonObject>& Params);

	/** megalith_update — Check or install updates */
	static FMegalithActionResult HandleUpdate(const TSharedPtr<FJsonObject>& Params);

	/** megalith_reindex — Trigger full project re-index */
	static FMegalithActionResult HandleReindex(const TSharedPtr<FJsonObject>& Params);
};
