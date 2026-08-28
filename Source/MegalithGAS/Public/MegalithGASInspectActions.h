#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASInspectActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 3: export_gas_manifest (moved from Phase 4 — operates on assets, not runtime)
	static FMegalithActionResult HandleExportGASManifest(const TSharedPtr<FJsonObject>& Params);
	// Phase 4: Runtime Debug
	static FMegalithActionResult HandleSnapshotGASState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetTagState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCooldownState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleTraceAbilityActivation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompareGASStates(const TSharedPtr<FJsonObject>& Params);
};
