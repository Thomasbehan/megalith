#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Motion Matching Action Pack — Sprint 5 (Pillar D): character/actor Blueprint
 * scaffolding for a UE 5.7 Motion Matching (Pose Search) setup.
 *
 * Namespace: "blueprint". Registered from FMegalithBlueprintModule::StartupModule.
 *
 * These actions wire an AnimBP + CharacterMovementComponent preset + Enhanced Input
 * onto a character Blueprint. Trajectory is AnimBP-side (Pose History node
 * bGenerateTrajectory) — there is NO CharacterTrajectoryComponent in 5.7, so
 * add_engine_component_typed deliberately does NOT special-case it.
 *
 * All handlers run on the game thread (MCP dispatch). Editor-time asset authoring
 * only — nothing replicated.
 */
class FMegalithMotionMatchingScaffoldActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// 5.1 — point a Blueprint's skeletal mesh component at an AnimBP's generated class.
	static FMegalithActionResult HandleSetAnimClass(const TSharedPtr<FJsonObject>& Params);

	// 5.2 — apply a CharacterMovementComponent locomotion preset to the BP CDO.
	static FMegalithActionResult HandleApplyMovementPreset(const TSharedPtr<FJsonObject>& Params);

	// 5.3 — resolve any UActorComponent subclass by friendly name and add it.
	static FMegalithActionResult HandleAddEngineComponentTyped(const TSharedPtr<FJsonObject>& Params);

	// 5.4 — create an IMC + per-action IA assets and bind them to AddMovementInput.
	static FMegalithActionResult HandleScaffoldLocomotionInput(const TSharedPtr<FJsonObject>& Params);

	// 5.5 — reflection-walk the AnimBP exposed-variable contract vs the BP variables. No mutation.
	static FMegalithActionResult HandleValidateAnimBpVariableContract(const TSharedPtr<FJsonObject>& Params);

	// 5.6 — COMPOSITE: create/reparent BP, set mesh, then call 5.1 + 5.2.
	static FMegalithActionResult HandleScaffoldMotionMatchingCharacter(const TSharedPtr<FJsonObject>& Params);

	// 5.7 — READ: report the effective value(s) of a component override on a child BP,
	// resolving the effective template (CDO subobject for native, ICH for SCS-inherited),
	// reading the requested property(ies) by reflection, and classifying the override source.
	static FMegalithActionResult HandleGetInheritedComponentOverride(const TSharedPtr<FJsonObject>& Params);

	// -----------------------------------------------------------------------
	// Pack A — thread-safe AnimBP event-graph authoring (anim-aware composites
	// over the generic override_parent_function / add_property_access /
	// add_nodes_bulk / connect_pins_bulk / add_variable primitives).
	// -----------------------------------------------------------------------

	// A.1 — override UAnimInstance::BlueprintThreadSafeUpdateAnimation on the AnimBP
	// (via override_parent_function) and return the entry node so callers chain logic.
	static FMegalithActionResult HandleScaffoldThreadSafeUpdate(const TSharedPtr<FJsonObject>& Params);

	// A.2 — author TryGetPawnOwner + cast-to-Character inside the thread-safe update
	// graph; return the cast result pin name so downstream nodes can read pawn/CMC members.
	static FMegalithActionResult HandleAddPawnOwnerAccess(const TSharedPtr<FJsonObject>& Params);

	// A.3 — COMPOSITE: ensure thread-safe update + pawn access exist, then create the
	// locomotion anim variables and author nodes that read them from the pawn/CMC and SET them.
	static FMegalithActionResult HandleScaffoldLocomotionAnimValues(const TSharedPtr<FJsonObject>& Params);

	// Pack D — CMC speed-band tuning on a Character BP's movement component CDO
	// (same persistence handshake as apply_movement_preset).
	static FMegalithActionResult HandleApplyLocomotionSpeedBand(const TSharedPtr<FJsonObject>& Params);
};
