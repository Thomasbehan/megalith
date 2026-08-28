#pragma once

#include "CoreMinimal.h"

class FMegalithToolRegistry;
struct FMegalithActionResult;
class FJsonObject;
class USoundCue;
class USoundNode;

class FMegalithAudioSoundCueActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// --- CRUD (10) ---
	static FMegalithActionResult CreateSoundCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundCueGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddSoundCueNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RemoveSoundCueNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ConnectSoundCueNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSoundCueFirstNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetSoundCueNodeProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListSoundCueNodeTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindSoundWavesInCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ValidateSoundCue(const TSharedPtr<FJsonObject>& Params);

	// --- Build & Templates (8) ---
	static FMegalithActionResult BuildSoundCueFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateRandomSoundCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateLayeredSoundCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateLoopingAmbientCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateDistanceCrossfadeCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateSwitchSoundCue(const TSharedPtr<FJsonObject>& Params);

	// --- Utility (5) ---
	static FMegalithActionResult DuplicateSoundCue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DeleteAudioAsset(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult PreviewSound(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult StopPreview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetSoundCueDuration(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers ---

	/** Get the node type registry: string name -> UClass* */
	static const TMap<FString, UClass*>& GetNodeTypeRegistry();

	/** Resolve a node type string to a UClass* */
	static UClass* ResolveNodeType(const FString& TypeName);

	/** Build a node ID string from a USoundNode (index-based) */
	static FString MakeNodeId(USoundCue* Cue, USoundNode* Node);

	/** Find a node by ID string within a cue */
	static USoundNode* FindNodeById(USoundCue* Cue, const FString& NodeId);

	/** Serialize a single sound node to JSON */
	static TSharedPtr<FJsonObject> SerializeNode(USoundCue* Cue, USoundNode* Node);

	/** Set a property on a sound node via reflection (with SoundWave special case) */
	static bool SetNodeProperty(USoundNode* Node, const FString& PropName, const TSharedPtr<FJsonValue>& Value, FString& OutError);

	/** Load a USoundCue by path */
	static USoundCue* LoadSoundCue(const FString& AssetPath, FString& OutError);

	/** Finalize a sound cue after graph modifications */
	static void FinalizeCue(USoundCue* Cue);

	/** Create a sound cue package + empty cue object */
	static USoundCue* CreateEmptySoundCue(const FString& AssetPath, FString& OutError);

	/** Create WavePlayer nodes from an array of wave paths, return the nodes */
	static TArray<USoundNode*> CreateWavePlayerNodes(USoundCue* Cue, const TArray<FString>& WavePaths, FString& OutError);
};
