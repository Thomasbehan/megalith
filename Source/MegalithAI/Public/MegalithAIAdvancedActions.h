#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIAdvancedActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

#if WITH_MASSENTITY
private:
	static FMegalithActionResult HandleListMassEntityConfigs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMassEntityConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateMassEntityConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddMassTrait(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveMassTrait(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListMassTraits(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListMassProcessors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateMassEntityConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMassEntityStats(const TSharedPtr<FJsonObject>& Params);

#if WITH_ZONEGRAPH
	static FMegalithActionResult HandleListZoneGraphs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleQueryZoneLanes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetZoneLaneInfo(const TSharedPtr<FJsonObject>& Params);
#endif // WITH_ZONEGRAPH

#endif // WITH_MASSENTITY
};
