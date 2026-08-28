#pragma once

#include "MegalithAIInternal.h"

class UNavigationSystemV1;

class FMegalithAINavigationActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// 143-150: NavSystem / NavMesh config & build
	static FMegalithActionResult HandleGetNavSystemConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNavMeshConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNavMeshConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNavMeshStats(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNavBoundsVolume(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListNavBoundsVolumes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildNavigation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNavBuildStatus(const TSharedPtr<FJsonObject>& Params);

	// 151-156: Nav areas, modifiers, links
	static FMegalithActionResult HandleListNavAreas(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateNavArea(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNavModifierVolume(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNavLinkProxy(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureNavLink(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListNavLinks(const TSharedPtr<FJsonObject>& Params);

	// 157-161: Path queries
	static FMegalithActionResult HandleFindPath(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleTestPath(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleProjectPointToNavigation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetRandomNavigablePoint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleNavigationRaycast(const TSharedPtr<FJsonObject>& Params);

	// 162-166: Agent config, invokers, crowd, analysis
	static FMegalithActionResult HandleConfigureNavAgent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNavInvokerComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCrowdManagerConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCrowdManagerConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAnalyzeNavigationCoverage(const TSharedPtr<FJsonObject>& Params);

	// 167-168: Harness support — rebuild + validation
	static FMegalithActionResult HandleRebuildNavigation(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateNavPoints(const TSharedPtr<FJsonObject>& Params);

	// Helpers
	static UWorld* GetNavWorld();
	static FVector ParseVector(const TSharedPtr<FJsonObject>& Params, const FString& FieldName, bool& bOutFound);
	static TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V);

	// Pump the editor world tick in a bounded loop until navmesh generation
	// completes (no remaining build tasks and not in progress) or the deadline
	// elapses. Returns true if generation finished within the budget.
	// Game-thread only; drives async tile generation to completion without a busy-loop.
	static bool WaitForNavGenerationComplete(UWorld* World, UNavigationSystemV1* NavSys, double TimeoutSeconds, int32& OutTicks);
};
