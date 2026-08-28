#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 14: Level Design Core — Volumes, Properties, NavMesh, Selection, Snapping (7 actions)
 *
 * spawn_volume          — Spawn trigger/kill/pain/blocking/nav_modifier/audio/post_process volumes
 * get_actor_properties  — Read arbitrary UPROPERTY values via FProperty reflection
 * copy_actor_properties — Copy properties from source actor to targets
 * build_navmesh         — Trigger navigation mesh rebuild (synchronous)
 * select_actors         — Control editor selection + camera focus
 * snap_to_surface       — Directional trace with surface normal alignment
 * set_collision_preset  — Set collision profile on actor's root primitive component
 */
class FMegalithMeshVolumeActions
{
public:
	/** Register all 7 level design core actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult SpawnVolume(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetActorProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CopyActorProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BuildNavmesh(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SelectActors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SnapToSurface(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetCollisionPreset(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---

	/** Resolve a volume type string to its UClass. Returns nullptr + sets OutError if invalid. */
	static UClass* ResolveVolumeClass(const FString& TypeStr, FString& OutError);

	/** Read a single FProperty value as string via ExportText_Direct */
	static bool ExportPropertyValue(const FProperty* Prop, const void* ContainerPtr, FString& OutValue);

	/** Import a single FProperty value from string via ImportText_Direct */
	static bool ImportPropertyValue(FProperty* Prop, void* ContainerPtr, const FString& Value, UObject* OwnerObject);
};
