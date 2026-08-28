#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class UAnimMontage;
class UBlendSpace;
class UAnimBlueprint;
class UAnimSequence;
class USkeleton;
class USkeletalMesh;

/**
 * Animation domain action handlers for Megalith.
 * Ported from AnimationMCPReaderLibrary — 23 proven actions.
 */
class FMegalithAnimationActions
{
public:
	/** Register all animation actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// --- Montage Sections (4) ---
	static FMegalithActionResult HandleAddMontageSection(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteMontageSection(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSectionNext(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSectionTime(const TSharedPtr<FJsonObject>& Params);

	// --- BlendSpace Samples (5) ---
	static FMegalithActionResult HandleAddBlendSpaceSample(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleEditBlendSpaceSample(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteBlendSpaceSample(const TSharedPtr<FJsonObject>& Params);
	// Standalone resample+dirty for already-broken / externally-authored blend spaces.
	static FMegalithActionResult HandleBakeBlendSpace(const TSharedPtr<FJsonObject>& Params);
	// Sets bInterpolateUsingGrid + PreferredTriangulationDirection, then rebakes.
	static FMegalithActionResult HandleSetBlendSpaceInterpolation(const TSharedPtr<FJsonObject>& Params);

	// --- ABP Graph Reading (7) ---
	static FMegalithActionResult HandleGetStateMachines(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetStateInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetTransitions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBlendNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetLinkedLayers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetGraphs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNodes(const TSharedPtr<FJsonObject>& Params);
	// Enumerate AnimGraph EvaluateChooser (v1/v2) nodes + reflectively-resolved chooser asset;
	// optional recursive nested-tree expansion via the Phase-2 MegalithChooserTreeCollector.
	static FMegalithActionResult HandleGetAnimGraphChoosers(const TSharedPtr<FJsonObject>& Params);

	// --- Notify Editing (2) ---
	static FMegalithActionResult HandleSetNotifyTime(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNotifyDuration(const TSharedPtr<FJsonObject>& Params);

	// --- Bone Tracks (3) ---
	static FMegalithActionResult HandleSetBoneTrackKeys(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddBoneTrack(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveBoneTrack(const TSharedPtr<FJsonObject>& Params);

	// --- Bone Pose Copy (1) ---
	// Reads the evaluated pose (raw track + ref pose fallback) for a list of
	// bones at a given time on the source AnimSequence, then writes those
	// transforms as keys to the destination AnimSequence. Useful when a target
	// anim has T-pose / wrong values on a subset of bones (e.g. left arm) and
	// you want to import a clean pose from a working anim without touching
	// the rest of the target.
	static FMegalithActionResult HandleCopyBonePoseBetweenSequences(const TSharedPtr<FJsonObject>& Params);

	// --- Virtual Bones (2) ---
	static FMegalithActionResult HandleAddVirtualBone(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveVirtualBones(const TSharedPtr<FJsonObject>& Params);

	// --- Skeleton Info (2) ---
	static FMegalithActionResult HandleGetSkeletonInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSkeletalMeshInfo(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 1: Read Actions (8) ---
	static FMegalithActionResult HandleGetSequenceInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSequenceNotifies(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBoneTrackKeys(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListBoneTracks(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSequenceCurves(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetMontageInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBlendSpaceInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSkeletonSockets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSkeletonPreviewAttachedAssets(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBoneRefPose(const TSharedPtr<FJsonObject>& Params);
	// T3-2: animated-frame FK-composed transform for one bone (extends get_bone_ref_pose's
	// bind-pose compose to an evaluated animation frame/time).
	static FMegalithActionResult HandleGetAnimatedBoneTransform(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbpInfo(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 2: Notify CRUD (4) ---
	static FMegalithActionResult HandleAddNotify(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNotifyState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveNotify(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNotifyTrack(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 3: Curve CRUD (5) ---
	static FMegalithActionResult HandleListCurves(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddCurve(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveCurve(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCurveKeys(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCurveKeys(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 4: Skeleton + BlendSpace (6) ---
	static FMegalithActionResult HandleAddSocket(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSocket(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSocketTransform(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSkeletonCurves(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetBlendSpaceAxis(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRootMotionSettings(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 5: Creation + Montage (6) ---
	static FMegalithActionResult HandleCreateSequence(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateSequence(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateMontage(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetMontageBlend(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddMontageSlot(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetMontageSlot(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 7: Anim Modifiers + Composites (5) ---
	static FMegalithActionResult HandleApplyAnimModifier(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAnimModifiers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCompositeInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddCompositeSegment(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveCompositeSegment(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 8a: IKRig (5) ---
	static FMegalithActionResult HandleGetIKRigInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddIKSolver(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveIKSolver(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetRetargeterInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRetargetChainMapping(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 8b: Control Rig Read (2) ---
	static FMegalithActionResult HandleGetControlRigInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetControlRigVariables(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 8c: Control Rig Write (1) ---
	static FMegalithActionResult HandleAddControlRigElement(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 9: ABP Read Enhancements (2) ---
	static FMegalithActionResult HandleGetAbpVariables(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAbpLinkedAssets(const TSharedPtr<FJsonObject>& Params);

	// --- Skeleton Compatibility (3) ---
	// Wraps USkeleton::CompatibleSkeletons — required for playing UE4 mannequin
	// anims on UE5 SK_Mannequin meshes (and similar legacy/cross-skeleton flows)
	// without manual run_python boilerplate.
	static FMegalithActionResult HandleGetCompatibleSkeletons(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddCompatibleSkeleton(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveCompatibleSkeleton(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 10: ABP Write Experimental (3) ---
	static FMegalithActionResult HandleAddStateToMachine(const TSharedPtr<FJsonObject>& Params);
	// add_conduit — spawn a UAnimStateConduitNode (a transition hub) into a state
	// machine. Its BoundGraph is a transition-logic graph, NOT an anim/pose graph.
	static FMegalithActionResult HandleAddConduit(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddTransition(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetTransitionRule(const TSharedPtr<FJsonObject>& Params);
	// Phase 6 — read back a transition's authored rule (kind + operands + comparison).
	static FMegalithActionResult HandleGetTransitionRule(const TSharedPtr<FJsonObject>& Params);

	// --- State-machine editing: removal + entry re-point ---
	// State/transition nodes own a BoundGraph, so removal routes through
	// FBlueprintEditorUtils::RemoveNode -> DestroyNode (tears down the inner graph)
	// rather than a bare UEdGraph::RemoveNode.
	static FMegalithActionResult HandleRemoveAnimState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAnimEntryState(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveAnimTransition(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 16: State Machine Authoring (#13/#14) ---
	// create_state_machine — spawn a UAnimGraphNode_StateMachine into an ABP's anim graph.
	// build_state_machine  — declarative builder composing create + add states/transitions/rules.
	static FMegalithActionResult HandleCreateStateMachine(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildStateMachine(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 14: Notify Properties (1) ---
	static FMegalithActionResult HandleSetNotifyProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 15: Physics Assets + IK Chains (6) ---
	static FMegalithActionResult HandleGetPhysicsAssetInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetBodyProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetConstraintProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddRetargetChain(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveRetargetChain(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRetargetChainBones(const TSharedPtr<FJsonObject>& Params);

	// --- Retarget CREATE/RUN pack (4) ---
	// Creates the IK Rig + IK Retargeter assets and runs a cross-skeleton batch
	// retarget. Complements the existing IK Rig / retargeter MUTATION actions
	// (get_ikrig_info, add_ik_solver, get_retargeter_info, set_retarget_chain_mapping,
	// add/remove/set_retarget_chain*) which only operate on assets that already exist.
	static FMegalithActionResult HandleCreateIKRig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateIKRetargeter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRetargeterRigs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchRetargetAnimations(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 11: Asset Creation + Setup (7 in this file) ---
	static FMegalithActionResult HandleCreateBlendSpace(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateBlendSpace1D(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateAimOffset(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateAimOffset1D(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateComposite(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateAnimBlueprint(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompareSkeletons(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 12: Sequence Properties + Sync Markers (7) ---
	static FMegalithActionResult HandleSetSequenceProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAdditiveSettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCompressionSettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSyncMarkers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSyncMarker(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSyncMarker(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameSyncMarker(const TSharedPtr<FJsonObject>& Params);

	// derive_foot_sync_markers — auto-derive L/R foot-plant sync markers from data
	// already present in a clip, via a 5-signal availability cascade (first signal
	// that yields plants wins): existing markers -> footstep notifies -> contact_l/_r
	// curves -> Phase curve extrema -> component-space foot-bone speed minima. The
	// foot-speed fallback is a native port of UFootstepAnimEventsModifier's FootBoneSpeed
	// technique (GetAnimPoseAtTimeIntervals + GetBonePose World space), so the action is
	// project-agnostic and needs no per-project modifier-config assets. All thresholds,
	// bone names, marker names and notify-track patterns are overridable. Honours dry_run.
	static FMegalithActionResult HandleDeriveFootSyncMarkers(const TSharedPtr<FJsonObject>& Params);

	// --- Anim-node bindings: function (Gap 2) + pin property (Gap 12) ---
	// Function bindings live on UAnimGraphNode_Base's public FMemberReference
	// UPROPERTYs (InitialUpdate/BecomeRelevant/Update Function). Pin property
	// bindings live in the node's UAnimGraphNodeBinding_Base::PropertyBindings map
	// (unlinkable class — reached via FProperty reflection). Setters mirror the
	// engine's own validate-then-recompile handshake.
	static FMegalithActionResult HandleGetAnimNodeFunctionBindings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAnimNodeFunctionBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAnimNodePinBindings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAnimNodePinBinding(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 13: Batch Ops + Montage Completion (6) ---
	static FMegalithActionResult HandleBatchExecute(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddMontageAnimSegment(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCloneNotifySetup(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBulkAddNotify(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateMontageFromSections(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildSequenceFromPoses(const TSharedPtr<FJsonObject>& Params);
};
