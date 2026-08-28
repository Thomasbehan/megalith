#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintNodeActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleAddNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConnectPins(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDisconnectPins(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetPinDefault(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNodePosition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchExecute(const TSharedPtr<FJsonObject>& Params);

	// Wave 3 — Discovery & Resolution
	static FMegalithActionResult HandleResolveNode(const TSharedPtr<FJsonObject>& Params);

	// Wave 4 — Bulk Node Operations
	static FMegalithActionResult HandleAddNodesBulk(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConnectPinsBulk(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetPinDefaultsBulk(const TSharedPtr<FJsonObject>& Params);

	// Wave 5 — Scaffolding & Templates
	static FMegalithActionResult HandleAddTimeline(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddEventNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddCommentNode(const TSharedPtr<FJsonObject>& Params);

	// Phase 3A — Timeline read/edit
	static FMegalithActionResult HandleGetTimelineData(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddTimelineTrack(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetTimelineKeys(const TSharedPtr<FJsonObject>& Params);

	// Wave 7 — Advanced
	static FMegalithActionResult HandlePromotePinToVariable(const TSharedPtr<FJsonObject>& Params);

	// Phase 1 (gap #11) — Cross-class property access (foreign-class VariableGet/Set)
	static FMegalithActionResult HandleAddPropertyAccess(const TSharedPtr<FJsonObject>& Params);

	// Genuine thread-safe Property Access — reflective UK2Node_PropertyAccess authoring.
	// Unlike add_property_access (which emits a foreign-member VariableGet with a self
	// pin, non-thread-safe), this spawns a real K2Node_PropertyAccess whose path-based
	// read is resolved thread-safe (or game-thread-cached) by the AnimBP property-access
	// compiler. The class is MinimalAPI/unlinkable, so it is created reflectively.
	static FMegalithActionResult HandleAddPropertyAccessNode(const TSharedPtr<FJsonObject>& Params);
};
