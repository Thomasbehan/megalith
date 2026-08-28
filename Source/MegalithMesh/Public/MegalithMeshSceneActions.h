#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 2: Scene Manipulation Actions (8 actions)
 * Actor CRUD operations - spawn, move, duplicate, delete, query info.
 * Foundation for blockout system.
 */
class FMegalithMeshSceneActions
{
public:
	/** Register all 8 scene manipulation actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

	/** True when batch_execute is running — sub-actions skip their own undo transactions */
	static bool bBatchTransactionActive;

private:
	static FMegalithActionResult GetActorInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SpawnActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult MoveActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DuplicateActor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DeleteActors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GroupActors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetActorProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchExecute(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AlignActors(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SnapToFloor(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ManageFolders(const TSharedPtr<FJsonObject>& Params);
};
