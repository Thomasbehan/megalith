#include "MegalithMeshSceneActions.h"
#include "MegalithMeshUtils.h"
#include "MegalithToolRegistry.h"
#include "MegalithParamSchema.h"
#include "MegalithAssetUtils.h"
#include "MegalithJsonUtils.h"

#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/BlockingVolume.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Editor.h"
#include "Selection.h"
#include "UObject/UObjectGlobals.h"
#include "CollisionQueryParams.h"

// ============================================================================
// Static member
// ============================================================================

bool FMegalithMeshSceneActions::bBatchTransactionActive = false;

// ============================================================================
// Helpers
// ============================================================================

namespace SceneActionHelpers
{
	/** Make a JSON array from a FVector */
	TArray<TSharedPtr<FJsonValue>> VectorToJsonArray(const FVector& V)
	{
		TArray<TSharedPtr<FJsonValue>> Arr;
		Arr.Add(MakeShared<FJsonValueNumber>(V.X));
		Arr.Add(MakeShared<FJsonValueNumber>(V.Y));
		Arr.Add(MakeShared<FJsonValueNumber>(V.Z));
		return Arr;
	}

	/** Scoped undo transaction that respects bBatchTransactionActive */
	struct FScopedMeshTransaction
	{
		bool bOwnsTransaction;

		FScopedMeshTransaction(const FText& Description)
			: bOwnsTransaction(!FMegalithMeshSceneActions::bBatchTransactionActive)
		{
			if (bOwnsTransaction && GEditor)
			{
				GEditor->BeginTransaction(Description);
			}
		}

		~FScopedMeshTransaction()
		{
			if (bOwnsTransaction && GEditor)
			{
				GEditor->EndTransaction();
			}
		}

		void Cancel()
		{
			if (bOwnsTransaction && GEditor)
			{
				GEditor->CancelTransaction(0);
				bOwnsTransaction = false; // prevent EndTransaction in destructor
			}
		}
	};

	/** Convert a mobility enum to string */
	FString MobilityToString(EComponentMobility::Type Mobility)
	{
		switch (Mobility)
		{
		case EComponentMobility::Static:     return TEXT("Static");
		case EComponentMobility::Stationary: return TEXT("Stationary");
		case EComponentMobility::Movable:    return TEXT("Movable");
		default:                             return TEXT("Unknown");
		}
	}

	/** Parse mobility string to enum, returns false if invalid */
	bool ParseMobility(const FString& Str, EComponentMobility::Type& Out)
	{
		if (Str.Equals(TEXT("Static"), ESearchCase::IgnoreCase))         { Out = EComponentMobility::Static; return true; }
		if (Str.Equals(TEXT("Stationary"), ESearchCase::IgnoreCase))     { Out = EComponentMobility::Stationary; return true; }
		if (Str.Equals(TEXT("Movable"), ESearchCase::IgnoreCase))        { Out = EComponentMobility::Movable; return true; }
		return false;
	}
}

// ============================================================================
// Registration
// ============================================================================

void FMegalithMeshSceneActions::RegisterActions(FMegalithToolRegistry& Registry)
{
	Registry.RegisterAction(TEXT("mesh"), TEXT("get_actor_info"),
		TEXT("Get comprehensive info for an actor in the editor world (class, transform, mesh, materials, tags, components, bounds)"),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::GetActorInfo),
		FParamSchemaBuilder()
			.Required(TEXT("actor_name"), TEXT("string"), TEXT("Actor name or label"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("spawn_actor"),
		TEXT("Spawn an actor in the editor world. Path starting with '/' spawns StaticMeshActor with that mesh; otherwise spawns by class name."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::SpawnActor),
		FParamSchemaBuilder()
			.Required(TEXT("class_or_mesh"), TEXT("string"), TEXT("Asset path for mesh (starts with '/') or class name"))
			.Required(TEXT("location"), TEXT("array"), TEXT("World location [x, y, z]"))
			.Optional(TEXT("rotation"), TEXT("array"), TEXT("Rotation [pitch, yaw, roll]"), TEXT("[0,0,0]"))
			.Optional(TEXT("scale"), TEXT("array"), TEXT("Scale [x, y, z]"), TEXT("[1,1,1]"))
			.Optional(TEXT("name"), TEXT("string"), TEXT("Optional label for the spawned actor"))
			.OptionalAssetPath(TEXT("folder"), TEXT("Actor folder path in the outliner"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("move_actor"),
		TEXT("Move/rotate/scale an actor. Set relative=true to offset from current transform."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::MoveActor),
		FParamSchemaBuilder()
			.Required(TEXT("actor_name"), TEXT("string"), TEXT("Actor name or label"))
			.Optional(TEXT("location"), TEXT("array"), TEXT("Location [x, y, z]"))
			.Optional(TEXT("rotation"), TEXT("array"), TEXT("Rotation [pitch, yaw, roll]"))
			.Optional(TEXT("scale"), TEXT("array"), TEXT("Scale [x, y, z]"))
			.Optional(TEXT("relative"), TEXT("boolean"), TEXT("If true, add to current transform instead of replacing"), TEXT("false"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("duplicate_actor"),
		TEXT("Duplicate an actor in the editor world with an optional position offset"),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::DuplicateActor),
		FParamSchemaBuilder()
			.Required(TEXT("actor_name"), TEXT("string"), TEXT("Actor name or label to duplicate"))
			.Optional(TEXT("new_name"), TEXT("string"), TEXT("Label for the duplicate"))
			.Optional(TEXT("offset"), TEXT("array"), TEXT("Position offset [x, y, z]"), TEXT("[0,0,0]"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("delete_actors"),
		TEXT("Delete one or more actors from the editor world. Validates ALL exist before deleting ANY. Does NOT delete asset files."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::DeleteActors),
		FParamSchemaBuilder()
			.Required(TEXT("actor_names"), TEXT("array"), TEXT("Array of actor names or labels to delete"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("group_actors"),
		TEXT("Move actors into an outliner folder (creates folder hierarchy automatically)"),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::GroupActors),
		FParamSchemaBuilder()
			.Required(TEXT("actor_names"), TEXT("array"), TEXT("Array of actor names or labels"))
			.Required(TEXT("group_name"), TEXT("string"), TEXT("Folder path (e.g. 'Furniture/Kitchen')"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("set_actor_properties"),
		TEXT("Set properties on an actor (mobility, physics, collision, shadows, tags, mass)"),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::SetActorProperties),
		FParamSchemaBuilder()
			.Required(TEXT("actor_name"), TEXT("string"), TEXT("Actor name or label"))
			.Optional(TEXT("mobility"), TEXT("string"), TEXT("Static, Stationary, or Movable"))
			.Optional(TEXT("simulate_physics"), TEXT("boolean"), TEXT("Enable/disable physics simulation"))
			.Optional(TEXT("collision_preset"), TEXT("string"), TEXT("Collision profile name"))
			.Optional(TEXT("cast_shadow"), TEXT("boolean"), TEXT("Enable/disable shadow casting"))
			.Optional(TEXT("tags"), TEXT("array"), TEXT("Array of tag strings to set on the actor"))
			.Optional(TEXT("mass_kg"), TEXT("number"), TEXT("Mass override in kg"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("batch_execute"),
		TEXT("Execute multiple mesh actions in a single undo transaction. Max 200 actions. No nested batch_execute allowed."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::BatchExecute),
		FParamSchemaBuilder()
			.Required(TEXT("actions"), TEXT("array"), TEXT("Array of {action, params} objects"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("align_actors"),
		TEXT("Align multiple actors along an axis. Modes: min/max (snap to extremes), center (average), distribute (spread evenly between min/max)."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::AlignActors),
		FParamSchemaBuilder()
			.Required(TEXT("actor_names"), TEXT("array"), TEXT("Array of actor names or labels to align"))
			.Required(TEXT("axis"), TEXT("string"), TEXT("Axis to align on: X, Y, or Z"))
			.Required(TEXT("mode"), TEXT("string"), TEXT("Alignment mode: min, max, center, or distribute"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("snap_to_floor"),
		TEXT("Snap actors to the floor by tracing downward. Places the bottom of each actor on the hit surface."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::SnapToFloor),
		FParamSchemaBuilder()
			.Required(TEXT("actor_names"), TEXT("array"), TEXT("Array of actor names or labels to snap"))
			.Optional(TEXT("trace_distance"), TEXT("number"), TEXT("Maximum downward trace distance"), TEXT("10000"))
			.Build());

	Registry.RegisterAction(TEXT("mesh"), TEXT("manage_folders"),
		TEXT("Manage World Outliner folders: list, delete (move actors to root), rename, or move folders."),
		FMegalithActionHandler::CreateStatic(&FMegalithMeshSceneActions::ManageFolders),
		FParamSchemaBuilder()
			.Required(TEXT("sub_action"), TEXT("string"), TEXT("Action: list, delete, rename, move"))
			.OptionalAssetPath(TEXT("folder"), TEXT("Target folder path (for delete/rename/move)"))
			.OptionalAssetPath(TEXT("new_folder"), TEXT("New folder path (for rename/move destination)"))
			.Optional(TEXT("include_subfolders"), TEXT("boolean"), TEXT("Include actors in subfolders"), TEXT("true"))
			.Build());
}

// ============================================================================
// 1. get_actor_info
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::GetActorInfo(const TSharedPtr<FJsonObject>& Params)
{
	FString ActorName;
	if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: actor_name"));
	}

	FString Error;
	AActor* Actor = MegalithMeshUtils::FindActorByName(ActorName, Error);
	if (!Actor)
	{
		return FMegalithActionResult::Error(Error);
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("name"), Actor->GetFName().ToString());
	Result->SetStringField(TEXT("class"), Actor->GetClass()->GetName());
	Result->SetStringField(TEXT("label"), Actor->GetActorLabel());

	// Transform
	Result->SetObjectField(TEXT("transform"), MegalithMeshUtils::TransformToJson(Actor->GetActorTransform()));

	// Mesh path and materials (if StaticMeshActor or has StaticMeshComponent)
	UStaticMeshComponent* SMC = Actor->FindComponentByClass<UStaticMeshComponent>();
	if (SMC)
	{
		if (UStaticMesh* Mesh = SMC->GetStaticMesh())
		{
			Result->SetStringField(TEXT("mesh"), Mesh->GetPathName());
		}

		TArray<TSharedPtr<FJsonValue>> MatArr;
		for (int32 i = 0; i < SMC->GetNumMaterials(); ++i)
		{
			UMaterialInterface* Mat = SMC->GetMaterial(i);
			MatArr.Add(MakeShared<FJsonValueString>(Mat ? Mat->GetPathName() : TEXT("None")));
		}
		Result->SetArrayField(TEXT("materials"), MatArr);
	}

	// Mobility
	USceneComponent* Root = Actor->GetRootComponent();
	if (Root)
	{
		Result->SetStringField(TEXT("mobility"), SceneActionHelpers::MobilityToString(Root->Mobility));
	}

	// Tags
	TArray<TSharedPtr<FJsonValue>> TagArr;
	for (const FName& Tag : Actor->Tags)
	{
		TagArr.Add(MakeShared<FJsonValueString>(Tag.ToString()));
	}
	Result->SetArrayField(TEXT("tags"), TagArr);

	// Components
	TArray<TSharedPtr<FJsonValue>> CompArr;
	TArray<UActorComponent*> Components;
	Actor->GetComponents(Components);
	for (UActorComponent* Comp : Components)
	{
		auto CompObj = MakeShared<FJsonObject>();
		CompObj->SetStringField(TEXT("name"), Comp->GetFName().ToString());
		CompObj->SetStringField(TEXT("class"), Comp->GetClass()->GetName());
		CompArr.Add(MakeShared<FJsonValueObject>(CompObj));
	}
	Result->SetArrayField(TEXT("components"), CompArr);

	// World bounds (for actors with a mesh)
	if (SMC && SMC->GetStaticMesh())
	{
		FBoxSphereBounds WorldBounds = SMC->Bounds;
		Result->SetObjectField(TEXT("world_bounds"), MegalithMeshUtils::BoundsToJson(WorldBounds));
	}

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 2. spawn_actor
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::SpawnActor(const TSharedPtr<FJsonObject>& Params)
{
	FString ClassOrMesh;
	if (!Params->TryGetStringField(TEXT("class_or_mesh"), ClassOrMesh))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: class_or_mesh"));
	}

	FVector Location;
	if (!MegalithMeshUtils::ParseVector(Params, TEXT("location"), Location))
	{
		return FMegalithActionResult::Error(TEXT("Missing or invalid required param: location (array of 3 numbers)"));
	}

	FRotator Rotation(0, 0, 0);
	MegalithMeshUtils::ParseRotator(Params, TEXT("rotation"), Rotation);

	FVector Scale(1, 1, 1);
	MegalithMeshUtils::ParseVector(Params, TEXT("scale"), Scale);

	FString OptionalName;
	Params->TryGetStringField(TEXT("name"), OptionalName);

	FString Folder;
	Params->TryGetStringField(TEXT("folder"), Folder);

	UWorld* World = MegalithMeshUtils::GetEditorWorld();
	if (!World)
	{
		return FMegalithActionResult::Error(TEXT("No editor world available"));
	}

	// Validate inputs before opening a transaction
	UStaticMesh* MeshToSpawn = nullptr;
	UClass* ClassToSpawn = nullptr;

	if (ClassOrMesh.StartsWith(TEXT("/")))
	{
		MeshToSpawn = FMegalithAssetUtils::LoadAssetByPath<UStaticMesh>(ClassOrMesh);
		if (!MeshToSpawn)
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("StaticMesh not found: %s"), *ClassOrMesh));
		}
	}
	else
	{
		// Block ABlockingVolume
		if (ClassOrMesh.Equals(TEXT("BlockingVolume"), ESearchCase::IgnoreCase) ||
			ClassOrMesh.Equals(TEXT("ABlockingVolume"), ESearchCase::IgnoreCase))
		{
			return FMegalithActionResult::Error(TEXT("Spawning BlockingVolume is not allowed via Megalith. Use the editor directly."));
		}

		ClassToSpawn = FindFirstObject<UClass>(*ClassOrMesh, EFindFirstObjectOptions::NativeFirst);
		if (!ClassToSpawn)
		{
			FString AltName = ClassOrMesh.StartsWith(TEXT("A")) ? ClassOrMesh.Mid(1) : (TEXT("A") + ClassOrMesh);
			ClassToSpawn = FindFirstObject<UClass>(*AltName, EFindFirstObjectOptions::NativeFirst);
		}
		if (!ClassToSpawn)
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("Class not found: %s"), *ClassOrMesh));
		}
		if (!ClassToSpawn->IsChildOf(AActor::StaticClass()))
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("Class '%s' is not an Actor class"), *ClassOrMesh));
		}
		if (ClassToSpawn->IsChildOf(ABlockingVolume::StaticClass()))
		{
			return FMegalithActionResult::Error(TEXT("Spawning BlockingVolume is not allowed via Megalith. Use the editor directly."));
		}
	}

	// All validation passed — open transaction and spawn
	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Spawn Actor")));

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedActor = nullptr;
	FString SpawnedClassName;

	if (MeshToSpawn)
	{
		AStaticMeshActor* SMActor = World->SpawnActor<AStaticMeshActor>(Location, Rotation, SpawnParams);
		if (!SMActor)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Failed to spawn StaticMeshActor"));
		}
		SMActor->GetStaticMeshComponent()->SetStaticMesh(MeshToSpawn);
		SMActor->SetActorScale3D(Scale);
		SpawnedActor = SMActor;
		SpawnedClassName = TEXT("StaticMeshActor");
	}
	else
	{
		SpawnedActor = World->SpawnActor(ClassToSpawn, &Location, &Rotation, SpawnParams);
		if (!SpawnedActor)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(FString::Printf(TEXT("Failed to spawn actor of class '%s'"), *ClassOrMesh));
		}
		SpawnedActor->SetActorScale3D(Scale);
		SpawnedClassName = ClassToSpawn->GetName();
	}

	// Set optional label
	if (!OptionalName.IsEmpty())
	{
		SpawnedActor->SetActorLabel(OptionalName);
	}

	// Set folder — default to /Spawned if none specified
	if (!Folder.IsEmpty())
	{
		SpawnedActor->SetFolderPath(FName(*Folder));
	}
	else
	{
		SpawnedActor->SetFolderPath(FName(TEXT("Spawned")));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("actor_name"), SpawnedActor->GetActorNameOrLabel());
	Result->SetStringField(TEXT("class"), SpawnedClassName);
	Result->SetArrayField(TEXT("location"), SceneActionHelpers::VectorToJsonArray(SpawnedActor->GetActorLocation()));

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 3. move_actor
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::MoveActor(const TSharedPtr<FJsonObject>& Params)
{
	FString ActorName;
	if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: actor_name"));
	}

	FString Error;
	AActor* Actor = MegalithMeshUtils::FindActorByName(ActorName, Error);
	if (!Actor)
	{
		return FMegalithActionResult::Error(Error);
	}

	bool bRelative = false;
	Params->TryGetBoolField(TEXT("relative"), bRelative);

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Move Actor")));

	FVector Location;
	if (MegalithMeshUtils::ParseVector(Params, TEXT("location"), Location))
	{
		if (bRelative)
		{
			Actor->SetActorLocation(Actor->GetActorLocation() + Location);
		}
		else
		{
			Actor->SetActorLocation(Location);
		}
	}

	FRotator Rotation;
	if (MegalithMeshUtils::ParseRotator(Params, TEXT("rotation"), Rotation))
	{
		if (bRelative)
		{
			Actor->SetActorRotation(Actor->GetActorRotation() + Rotation);
		}
		else
		{
			Actor->SetActorRotation(Rotation);
		}
	}

	FVector Scale;
	if (MegalithMeshUtils::ParseVector(Params, TEXT("scale"), Scale))
	{
		if (bRelative)
		{
			Actor->SetActorScale3D(Actor->GetActorScale3D() + Scale);
		}
		else
		{
			Actor->SetActorScale3D(Scale);
		}
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("actor_name"), Actor->GetActorNameOrLabel());
	Result->SetObjectField(TEXT("new_transform"), MegalithMeshUtils::TransformToJson(Actor->GetActorTransform()));

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 4. duplicate_actor
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::DuplicateActor(const TSharedPtr<FJsonObject>& Params)
{
	FString ActorName;
	if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: actor_name"));
	}

	FString Error;
	AActor* SourceActor = MegalithMeshUtils::FindActorByName(ActorName, Error);
	if (!SourceActor)
	{
		return FMegalithActionResult::Error(Error);
	}

	FVector Offset(0, 0, 0);
	MegalithMeshUtils::ParseVector(Params, TEXT("offset"), Offset);

	FString NewName;
	Params->TryGetStringField(TEXT("new_name"), NewName);

	UWorld* World = MegalithMeshUtils::GetEditorWorld();
	if (!World)
	{
		return FMegalithActionResult::Error(TEXT("No editor world available"));
	}

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Duplicate Actor")));

	// Select the source actor and duplicate via editor
	GEditor->SelectNone(false, true, false);
	GEditor->SelectActor(SourceActor, true, false, true);

	// Use editor duplication
	GEditor->edactDuplicateSelected(World->GetCurrentLevel(), false);

	// The duplicated actor is now the only selected actor
	AActor* DupActor = nullptr;
	USelection* SelectedActors = GEditor->GetSelectedActors();
	for (int32 i = 0; i < SelectedActors->Num(); ++i)
	{
		AActor* Selected = Cast<AActor>(SelectedActors->GetSelectedObject(i));
		if (Selected && Selected != SourceActor)
		{
			DupActor = Selected;
			break;
		}
	}

	if (!DupActor)
	{
		Transaction.Cancel();
		return FMegalithActionResult::Error(TEXT("Failed to duplicate actor"));
	}

	// Apply offset
	DupActor->SetActorLocation(DupActor->GetActorLocation() + Offset);

	// Set label if requested
	if (!NewName.IsEmpty())
	{
		DupActor->SetActorLabel(NewName);
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("original"), SourceActor->GetActorNameOrLabel());
	Result->SetStringField(TEXT("duplicate"), DupActor->GetActorNameOrLabel());
	Result->SetArrayField(TEXT("location"), SceneActionHelpers::VectorToJsonArray(DupActor->GetActorLocation()));

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 5. delete_actors
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::DeleteActors(const TSharedPtr<FJsonObject>& Params)
{
	const TArray<TSharedPtr<FJsonValue>>* NamesArr;
	if (!Params->TryGetArrayField(TEXT("actor_names"), NamesArr) || NamesArr->Num() == 0)
	{
		return FMegalithActionResult::Error(TEXT("Missing or empty required param: actor_names (array of strings)"));
	}

	// Phase 1: resolve ALL actors first, fail if any are missing
	TArray<AActor*> Actors;
	Actors.Reserve(NamesArr->Num());

	for (const auto& Val : *NamesArr)
	{
		FString Name = Val->AsString();
		FString Error;
		AActor* Actor = MegalithMeshUtils::FindActorByName(Name, Error);
		if (!Actor)
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("Actor not found: %s. No actors deleted."), *Name));
		}
		Actors.Add(Actor);
	}

	// Phase 2: delete all via editor selection (undo-compatible)
	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Delete Actors")));

	TArray<TSharedPtr<FJsonValue>> DeletedNames;

	// Collect names before deletion
	for (AActor* Actor : Actors)
	{
		DeletedNames.Add(MakeShared<FJsonValueString>(Actor->GetActorNameOrLabel()));
	}

	// Select all targets and delete via editor (participates in undo)
	GEditor->SelectNone(false, true, false);
	for (AActor* Actor : Actors)
	{
		GEditor->SelectActor(Actor, true, false, true);
	}
	// bVerifyDeletionCanHappen=false to suppress editor confirmation dialogs
	// bWarnAboutReferences=false, bWarnAboutSoftReferences=false for programmatic use
	GEditor->edactDeleteSelected(MegalithMeshUtils::GetEditorWorld(), false, false, false);

	auto Result = MakeShared<FJsonObject>();
	Result->SetNumberField(TEXT("deleted"), DeletedNames.Num());
	Result->SetArrayField(TEXT("actors"), DeletedNames);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 6. group_actors
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::GroupActors(const TSharedPtr<FJsonObject>& Params)
{
	const TArray<TSharedPtr<FJsonValue>>* NamesArr;
	if (!Params->TryGetArrayField(TEXT("actor_names"), NamesArr) || NamesArr->Num() == 0)
	{
		return FMegalithActionResult::Error(TEXT("Missing or empty required param: actor_names"));
	}

	FString GroupName;
	if (!Params->TryGetStringField(TEXT("group_name"), GroupName))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: group_name"));
	}

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Group Actors")));

	int32 Count = 0;
	FString FirstError;
	for (const auto& Val : *NamesArr)
	{
		FString Name = Val->AsString();
		FString Error;
		AActor* Actor = MegalithMeshUtils::FindActorByName(Name, Error);
		if (!Actor)
		{
			if (FirstError.IsEmpty())
			{
				FirstError = Error;
			}
			continue;
		}
		Actor->SetFolderPath(FName(*GroupName));
		Count++;
	}

	if (Count == 0 && !FirstError.IsEmpty())
	{
		Transaction.Cancel();
		return FMegalithActionResult::Error(FirstError);
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("group"), GroupName);
	Result->SetNumberField(TEXT("actors"), Count);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 7. set_actor_properties
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::SetActorProperties(const TSharedPtr<FJsonObject>& Params)
{
	FString ActorName;
	if (!Params->TryGetStringField(TEXT("actor_name"), ActorName))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: actor_name"));
	}

	FString Error;
	AActor* Actor = MegalithMeshUtils::FindActorByName(ActorName, Error);
	if (!Actor)
	{
		return FMegalithActionResult::Error(Error);
	}

	USceneComponent* Root = Actor->GetRootComponent();
	UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Root);

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Set Actor Properties")));

	TArray<TSharedPtr<FJsonValue>> PropertiesSet;

	// Mobility
	FString MobilityStr;
	if (Params->TryGetStringField(TEXT("mobility"), MobilityStr))
	{
		if (!Root)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Actor has no root component to set mobility on"));
		}
		EComponentMobility::Type Mob;
		if (!SceneActionHelpers::ParseMobility(MobilityStr, Mob))
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(FString::Printf(TEXT("Invalid mobility: '%s'. Use Static, Stationary, or Movable."), *MobilityStr));
		}
		Root->SetMobility(Mob);
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("mobility")));
	}

	// Simulate physics (must check mobility is Movable)
	bool bSimPhysics;
	if (Params->TryGetBoolField(TEXT("simulate_physics"), bSimPhysics))
	{
		if (!Primitive)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Actor's root component is not a PrimitiveComponent — cannot set physics"));
		}
		if (bSimPhysics && Root->Mobility != EComponentMobility::Movable)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Mobility must be Movable before enabling SimulatePhysics. Set mobility first."));
		}
		Primitive->SetSimulatePhysics(bSimPhysics);
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("simulate_physics")));
	}

	// Collision preset
	FString CollisionPreset;
	if (Params->TryGetStringField(TEXT("collision_preset"), CollisionPreset))
	{
		if (!Primitive)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Actor's root component is not a PrimitiveComponent — cannot set collision"));
		}
		Primitive->SetCollisionProfileName(FName(*CollisionPreset));
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("collision_preset")));
	}

	// Cast shadow
	bool bCastShadow;
	if (Params->TryGetBoolField(TEXT("cast_shadow"), bCastShadow))
	{
		if (!Primitive)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Actor's root component is not a PrimitiveComponent — cannot set shadow"));
		}
		Primitive->SetCastShadow(bCastShadow);
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("cast_shadow")));
	}

	// Tags
	const TArray<TSharedPtr<FJsonValue>>* TagsArr;
	if (Params->TryGetArrayField(TEXT("tags"), TagsArr))
	{
		Actor->Tags.Empty();
		for (const auto& TagVal : *TagsArr)
		{
			Actor->Tags.Add(FName(*TagVal->AsString()));
		}
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("tags")));
	}

	// Mass override
	double MassKg;
	if (Params->TryGetNumberField(TEXT("mass_kg"), MassKg))
	{
		if (!Primitive)
		{
			Transaction.Cancel();
			return FMegalithActionResult::Error(TEXT("Actor's root component is not a PrimitiveComponent — cannot set mass"));
		}
		Primitive->SetMassOverrideInKg(NAME_None, static_cast<float>(MassKg), true);
		PropertiesSet.Add(MakeShared<FJsonValueString>(TEXT("mass_kg")));
	}

	if (PropertiesSet.Num() == 0)
	{
		Transaction.Cancel();
		return FMegalithActionResult::Error(TEXT("No properties provided to set. Specify at least one of: mobility, simulate_physics, collision_preset, cast_shadow, tags, mass_kg"));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("actor_name"), Actor->GetActorNameOrLabel());
	Result->SetArrayField(TEXT("properties_set"), PropertiesSet);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 8. batch_execute
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::BatchExecute(const TSharedPtr<FJsonObject>& Params)
{
	// Reject nested batch_execute
	if (bBatchTransactionActive)
	{
		return FMegalithActionResult::Error(TEXT("Nested batch_execute is not allowed"));
	}

	const TArray<TSharedPtr<FJsonValue>>* ActionsArr;
	if (!Params->TryGetArrayField(TEXT("actions"), ActionsArr) || ActionsArr->Num() == 0)
	{
		return FMegalithActionResult::Error(TEXT("Missing or empty required param: actions"));
	}

	if (ActionsArr->Num() > 200)
	{
		return FMegalithActionResult::Error(FString::Printf(
			TEXT("batch_execute is capped at 200 actions, got %d"), ActionsArr->Num()));
	}

	// Begin single outer transaction
	if (GEditor)
	{
		GEditor->BeginTransaction(FText::FromString(TEXT("Megalith: Batch Execute")));
	}
	bBatchTransactionActive = true;

	int32 Succeeded = 0;
	int32 Failed = 0;
	TArray<TSharedPtr<FJsonValue>> ResultsArr;
	bool bHadFailure = false;

	for (int32 i = 0; i < ActionsArr->Num(); ++i)
	{
		const TSharedPtr<FJsonObject>* ActionObj;
		if (!(*ActionsArr)[i]->TryGetObject(ActionObj))
		{
			auto ItemResult = MakeShared<FJsonObject>();
			ItemResult->SetStringField(TEXT("action"), TEXT("unknown"));
			ItemResult->SetBoolField(TEXT("success"), false);
			ItemResult->SetStringField(TEXT("error"), TEXT("Invalid action entry — expected object with 'action' and 'params' fields"));
			ResultsArr.Add(MakeShared<FJsonValueObject>(ItemResult));
			Failed++;
			bHadFailure = true;
			break;
		}

		FString ActionName;
		if (!(*ActionObj)->TryGetStringField(TEXT("action"), ActionName))
		{
			auto ItemResult = MakeShared<FJsonObject>();
			ItemResult->SetStringField(TEXT("action"), TEXT("unknown"));
			ItemResult->SetBoolField(TEXT("success"), false);
			ItemResult->SetStringField(TEXT("error"), TEXT("Missing 'action' field in batch item"));
			ResultsArr.Add(MakeShared<FJsonValueObject>(ItemResult));
			Failed++;
			bHadFailure = true;
			break;
		}

		// Reject nested batch_execute
		if (ActionName == TEXT("batch_execute"))
		{
			auto ItemResult = MakeShared<FJsonObject>();
			ItemResult->SetStringField(TEXT("action"), ActionName);
			ItemResult->SetBoolField(TEXT("success"), false);
			ItemResult->SetStringField(TEXT("error"), TEXT("Nested batch_execute is not allowed"));
			ResultsArr.Add(MakeShared<FJsonValueObject>(ItemResult));
			Failed++;
			bHadFailure = true;
			break;
		}

		// Get params (optional, default to empty object)
		TSharedPtr<FJsonObject> ActionParams = MakeShared<FJsonObject>();
		const TSharedPtr<FJsonObject>* ParamsObj;
		if ((*ActionObj)->TryGetObjectField(TEXT("params"), ParamsObj))
		{
			ActionParams = *ParamsObj;
		}

		// Dispatch via registry
		FMegalithActionResult ActionResult = FMegalithToolRegistry::Get().ExecuteAction(TEXT("mesh"), ActionName, ActionParams);

		auto ItemResult = MakeShared<FJsonObject>();
		ItemResult->SetStringField(TEXT("action"), ActionName);
		ItemResult->SetBoolField(TEXT("success"), ActionResult.bSuccess);
		if (!ActionResult.bSuccess)
		{
			ItemResult->SetStringField(TEXT("error"), ActionResult.ErrorMessage);
			ResultsArr.Add(MakeShared<FJsonValueObject>(ItemResult));
			Failed++;
			bHadFailure = true;
			break; // Cancel on first failure
		}
		Succeeded++;
		ResultsArr.Add(MakeShared<FJsonValueObject>(ItemResult));
	}

	// End or cancel transaction
	bBatchTransactionActive = false;
	if (GEditor)
	{
		if (bHadFailure)
		{
			GEditor->CancelTransaction(0);
		}
		else
		{
			GEditor->EndTransaction();
		}
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetNumberField(TEXT("total"), ActionsArr->Num());
	Result->SetNumberField(TEXT("succeeded"), Succeeded);
	Result->SetNumberField(TEXT("failed"), Failed);
	Result->SetArrayField(TEXT("results"), ResultsArr);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 9. align_actors
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::AlignActors(const TSharedPtr<FJsonObject>& Params)
{
	const TArray<TSharedPtr<FJsonValue>>* NamesArr;
	if (!Params->TryGetArrayField(TEXT("actor_names"), NamesArr) || NamesArr->Num() < 2)
	{
		return FMegalithActionResult::Error(TEXT("Missing or insufficient actor_names (need at least 2 actors to align)"));
	}

	FString AxisStr;
	if (!Params->TryGetStringField(TEXT("axis"), AxisStr))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: axis (X, Y, or Z)"));
	}
	AxisStr = AxisStr.ToUpper();
	int32 AxisIndex = -1;
	if (AxisStr == TEXT("X")) AxisIndex = 0;
	else if (AxisStr == TEXT("Y")) AxisIndex = 1;
	else if (AxisStr == TEXT("Z")) AxisIndex = 2;
	else
	{
		return FMegalithActionResult::Error(FString::Printf(TEXT("Invalid axis: '%s'. Use X, Y, or Z."), *AxisStr));
	}

	FString ModeStr;
	if (!Params->TryGetStringField(TEXT("mode"), ModeStr))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: mode (min, max, center, or distribute)"));
	}
	ModeStr = ModeStr.ToLower();
	if (ModeStr != TEXT("min") && ModeStr != TEXT("max") && ModeStr != TEXT("center") && ModeStr != TEXT("distribute"))
	{
		return FMegalithActionResult::Error(FString::Printf(TEXT("Invalid mode: '%s'. Use min, max, center, or distribute."), *ModeStr));
	}

	// Resolve all actors first — fail if any missing
	TArray<AActor*> Actors;
	Actors.Reserve(NamesArr->Num());
	for (const auto& Val : *NamesArr)
	{
		FString Name = Val->AsString();
		FString Error;
		AActor* Actor = MegalithMeshUtils::FindActorByName(Name, Error);
		if (!Actor)
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("Actor not found: %s"), *Name));
		}
		Actors.Add(Actor);
	}

	// Lambdas for axis access
	auto GetAxisValue = [AxisIndex](const FVector& V) -> double
	{
		return (AxisIndex == 0) ? V.X : (AxisIndex == 1) ? V.Y : V.Z;
	};

	auto SetAxisValue = [AxisIndex](FVector& V, double Val)
	{
		if (AxisIndex == 0) V.X = Val;
		else if (AxisIndex == 1) V.Y = Val;
		else V.Z = Val;
	};

	// Compute stats along the target axis
	double MinVal = TNumericLimits<double>::Max();
	double MaxVal = TNumericLimits<double>::Lowest();
	double Sum = 0.0;
	for (AActor* Actor : Actors)
	{
		double Val = GetAxisValue(Actor->GetActorLocation());
		MinVal = FMath::Min(MinVal, Val);
		MaxVal = FMath::Max(MaxVal, Val);
		Sum += Val;
	}

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Align Actors")));

	TArray<TSharedPtr<FJsonValue>> ResultArr;

	if (ModeStr == TEXT("distribute"))
	{
		// Sort actors by current position on the target axis, then spread evenly
		struct FActorSortEntry
		{
			AActor* Actor;
			double AxisVal;
		};
		TArray<FActorSortEntry> Sorted;
		Sorted.Reserve(Actors.Num());
		for (AActor* Actor : Actors)
		{
			Sorted.Add({ Actor, GetAxisValue(Actor->GetActorLocation()) });
		}
		Sorted.Sort([](const FActorSortEntry& A, const FActorSortEntry& B) { return A.AxisVal < B.AxisVal; });

		double Range = MaxVal - MinVal;
		int32 Count = Sorted.Num();

		for (int32 i = 0; i < Count; ++i)
		{
			AActor* Actor = Sorted[i].Actor;
			double NewVal = (Count > 1) ? (MinVal + (Range * i) / (Count - 1)) : MinVal;
			FVector Loc = Actor->GetActorLocation();
			SetAxisValue(Loc, NewVal);
			Actor->SetActorLocation(Loc);

			auto Entry = MakeShared<FJsonObject>();
			Entry->SetStringField(TEXT("actor"), Actor->GetActorNameOrLabel());
			Entry->SetArrayField(TEXT("new_location"), SceneActionHelpers::VectorToJsonArray(Loc));
			ResultArr.Add(MakeShared<FJsonValueObject>(Entry));
		}
	}
	else
	{
		double Target = 0.0;
		if (ModeStr == TEXT("min")) Target = MinVal;
		else if (ModeStr == TEXT("max")) Target = MaxVal;
		else /* center */ Target = Sum / Actors.Num();

		for (AActor* Actor : Actors)
		{
			FVector Loc = Actor->GetActorLocation();
			SetAxisValue(Loc, Target);
			Actor->SetActorLocation(Loc);

			auto Entry = MakeShared<FJsonObject>();
			Entry->SetStringField(TEXT("actor"), Actor->GetActorNameOrLabel());
			Entry->SetArrayField(TEXT("new_location"), SceneActionHelpers::VectorToJsonArray(Loc));
			ResultArr.Add(MakeShared<FJsonValueObject>(Entry));
		}
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("axis"), AxisStr);
	Result->SetStringField(TEXT("mode"), ModeStr);
	Result->SetNumberField(TEXT("aligned"), ResultArr.Num());
	Result->SetArrayField(TEXT("actors"), ResultArr);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// 10. snap_to_floor
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::SnapToFloor(const TSharedPtr<FJsonObject>& Params)
{
	const TArray<TSharedPtr<FJsonValue>>* NamesArr;
	if (!Params->TryGetArrayField(TEXT("actor_names"), NamesArr) || NamesArr->Num() == 0)
	{
		return FMegalithActionResult::Error(TEXT("Missing or empty required param: actor_names"));
	}

	double TraceDistance = 10000.0;
	Params->TryGetNumberField(TEXT("trace_distance"), TraceDistance);
	if (TraceDistance <= 0.0)
	{
		return FMegalithActionResult::Error(TEXT("trace_distance must be positive"));
	}

	// Resolve all actors first — fail if any missing
	TArray<AActor*> Actors;
	Actors.Reserve(NamesArr->Num());
	for (const auto& Val : *NamesArr)
	{
		FString Name = Val->AsString();
		FString Error;
		AActor* Actor = MegalithMeshUtils::FindActorByName(Name, Error);
		if (!Actor)
		{
			return FMegalithActionResult::Error(FString::Printf(TEXT("Actor not found: %s"), *Name));
		}
		Actors.Add(Actor);
	}

	UWorld* World = MegalithMeshUtils::GetEditorWorld();
	if (!World)
	{
		return FMegalithActionResult::Error(TEXT("No editor world available"));
	}

	SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Snap to Floor")));

	TArray<TSharedPtr<FJsonValue>> ResultArr;
	int32 Snapped = 0;
	int32 Missed = 0;

	for (AActor* Actor : Actors)
	{
		// Get actor bounds to compute half-height (distance from center to bottom)
		FVector BoundsOrigin, BoundsExtent;
		Actor->GetActorBounds(false, BoundsOrigin, BoundsExtent);
		double ActorHalfHeight = BoundsExtent.Z; // Extent is half-size per axis

		FVector ActorLoc = Actor->GetActorLocation();

		// Trace downward from actor location
		FVector TraceStart = ActorLoc;
		FVector TraceEnd = TraceStart - FVector(0, 0, TraceDistance);

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SnapToFloor), true);
		QueryParams.AddIgnoredActor(Actor);

		FHitResult Hit;
		bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

		auto Entry = MakeShared<FJsonObject>();
		Entry->SetStringField(TEXT("actor"), Actor->GetActorNameOrLabel());

		if (bHit)
		{
			// Place actor so its bottom sits on the hit point.
			// Account for pivot offset: the actor's pivot may not be at bounds center.
			// PivotOffset = ActorLocation.Z - BoundsOrigin.Z
			// NewZ = HitPoint.Z + ActorHalfHeight + PivotOffset
			double PivotOffset = ActorLoc.Z - BoundsOrigin.Z;
			double NewZ = Hit.ImpactPoint.Z + ActorHalfHeight + PivotOffset;
			FVector NewLoc = ActorLoc;
			NewLoc.Z = NewZ;
			Actor->SetActorLocation(NewLoc);

			Entry->SetArrayField(TEXT("new_location"), SceneActionHelpers::VectorToJsonArray(NewLoc));
			Entry->SetNumberField(TEXT("floor_z"), Hit.ImpactPoint.Z);
			Entry->SetBoolField(TEXT("snapped"), true);
			Snapped++;
		}
		else
		{
			Entry->SetBoolField(TEXT("snapped"), false);
			Entry->SetStringField(TEXT("reason"), TEXT("No floor found within trace distance"));
			Missed++;
		}

		ResultArr.Add(MakeShared<FJsonValueObject>(Entry));
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetNumberField(TEXT("snapped"), Snapped);
	Result->SetNumberField(TEXT("missed"), Missed);
	Result->SetNumberField(TEXT("trace_distance"), TraceDistance);
	Result->SetArrayField(TEXT("actors"), ResultArr);

	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// manage_folders
// ============================================================================

FMegalithActionResult FMegalithMeshSceneActions::ManageFolders(const TSharedPtr<FJsonObject>& Params)
{
	FString SubAction;
	if (!Params->TryGetStringField(TEXT("sub_action"), SubAction))
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: sub_action (list, delete, rename, move)"));
	}
	SubAction = SubAction.ToLower();

	UWorld* World = MegalithMeshUtils::GetEditorWorld();
	if (!World)
	{
		return FMegalithActionResult::Error(TEXT("No editor world available"));
	}

	// ---- LIST: enumerate all folder paths in use ----
	if (SubAction == TEXT("list"))
	{
		TMap<FString, int32> FolderCounts;
		for (TActorIterator<AActor> It(World); It; ++It)
		{
			FName FolderPath = It->GetFolderPath();
			if (!FolderPath.IsNone())
			{
				FolderCounts.FindOrAdd(FolderPath.ToString())++;
			}
		}

		TArray<TSharedPtr<FJsonValue>> FolderArr;
		FolderCounts.KeySort([](const FString& A, const FString& B) { return A < B; });
		for (const auto& Pair : FolderCounts)
		{
			auto Obj = MakeShared<FJsonObject>();
			Obj->SetStringField(TEXT("folder"), Pair.Key);
			Obj->SetNumberField(TEXT("actor_count"), Pair.Value);
			FolderArr.Add(MakeShared<FJsonValueObject>(Obj));
		}

		auto Result = MakeShared<FJsonObject>();
		Result->SetArrayField(TEXT("folders"), FolderArr);
		Result->SetNumberField(TEXT("total_folders"), FolderArr.Num());
		return FMegalithActionResult::Success(Result);
	}

	// ---- Remaining sub-actions need a folder param ----
	FString Folder;
	if (!Params->TryGetStringField(TEXT("folder"), Folder) || Folder.IsEmpty())
	{
		return FMegalithActionResult::Error(TEXT("Missing required param: folder (for delete/rename/move)"));
	}

	bool bIncludeSubfolders = true;
	Params->TryGetBoolField(TEXT("include_subfolders"), bIncludeSubfolders);

	// Collect actors in the target folder
	TArray<AActor*> FolderActors;
	FName FolderFName(*Folder);
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		FName ActorFolder = It->GetFolderPath();
		if (ActorFolder == FolderFName)
		{
			FolderActors.Add(*It);
		}
		else if (bIncludeSubfolders && !ActorFolder.IsNone())
		{
			FString ActorFolderStr = ActorFolder.ToString();
			if (ActorFolderStr.StartsWith(Folder + TEXT("/")))
			{
				FolderActors.Add(*It);
			}
		}
	}

	// ---- DELETE: move all actors to root (folder vanishes when empty) ----
	if (SubAction == TEXT("delete"))
	{
		SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Delete Folder")));

		for (AActor* Actor : FolderActors)
		{
			Actor->SetFolderPath(NAME_None);
		}

		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("deleted_folder"), Folder);
		Result->SetNumberField(TEXT("actors_moved_to_root"), FolderActors.Num());
		return FMegalithActionResult::Success(Result);
	}

	// ---- RENAME: change folder path for all actors ----
	if (SubAction == TEXT("rename"))
	{
		FString NewFolder;
		if (!Params->TryGetStringField(TEXT("new_folder"), NewFolder) || NewFolder.IsEmpty())
		{
			return FMegalithActionResult::Error(TEXT("Missing required param: new_folder (for rename)"));
		}

		SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Rename Folder")));

		int32 Renamed = 0;
		for (AActor* Actor : FolderActors)
		{
			FString ActorFolderStr = Actor->GetFolderPath().ToString();
			if (ActorFolderStr == Folder)
			{
				Actor->SetFolderPath(FName(*NewFolder));
			}
			else if (bIncludeSubfolders)
			{
				// Preserve subfolder structure: "OldFolder/Sub" -> "NewFolder/Sub"
				FString Remainder = ActorFolderStr.Mid(Folder.Len());
				Actor->SetFolderPath(FName(*(NewFolder + Remainder)));
			}
			Renamed++;
		}

		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("old_folder"), Folder);
		Result->SetStringField(TEXT("new_folder"), NewFolder);
		Result->SetNumberField(TEXT("actors_renamed"), Renamed);
		return FMegalithActionResult::Success(Result);
	}

	// ---- MOVE: move all actors into a different folder ----
	if (SubAction == TEXT("move"))
	{
		FString NewFolder;
		if (!Params->TryGetStringField(TEXT("new_folder"), NewFolder) || NewFolder.IsEmpty())
		{
			return FMegalithActionResult::Error(TEXT("Missing required param: new_folder (for move destination)"));
		}

		SceneActionHelpers::FScopedMeshTransaction Transaction(FText::FromString(TEXT("Megalith: Move Folder")));

		for (AActor* Actor : FolderActors)
		{
			Actor->SetFolderPath(FName(*NewFolder));
		}

		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("source_folder"), Folder);
		Result->SetStringField(TEXT("destination_folder"), NewFolder);
		Result->SetNumberField(TEXT("actors_moved"), FolderActors.Num());
		return FMegalithActionResult::Success(Result);
	}

	return FMegalithActionResult::Error(FString::Printf(
		TEXT("Unknown sub_action: '%s'. Valid: list, delete, rename, move"), *SubAction));
}
