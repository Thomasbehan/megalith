#pragma once

#include "CoreMinimal.h"

#if WITH_METASOUND

class FMegalithToolRegistry;
struct FMegalithActionResult;

class FMegalithAudioMetaSoundActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// ---- Core CRUD (12) ----
	static FMegalithActionResult CreateMetaSoundSource(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateMetaSoundPatch(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddMetaSoundNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RemoveMetaSoundNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ConnectMetaSoundNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DisconnectMetaSoundNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddMetaSoundInput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddMetaSoundOutput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetMetaSoundInputDefault(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddMetaSoundInterface(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMetaSoundGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListMetaSoundConnections(const TSharedPtr<FJsonObject>& Params);

	// ---- Query & Discovery (5) ----
	static FMegalithActionResult ListAvailableMetaSoundNodes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMetaSoundNodeInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindMetaSoundNodeInputs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindMetaSoundNodeOutputs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMetaSoundInputNames(const TSharedPtr<FJsonObject>& Params);

	// ---- Build & Templates (8) ----
	static FMegalithActionResult BuildMetaSoundFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateMetaSoundPreset(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateOneShotSfx(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateLoopingAmbientMetaSound(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateSynthesizedTone(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateInteractiveMetaSound(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AddMetaSoundVariable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetMetaSoundNodeLocation(const TSharedPtr<FJsonObject>& Params);

	// ---- Helpers ----

	/** Split "/Game/Foo/Bar" into PackagePath="/Game/Foo" and AssetName="Bar" */
	static bool SplitAssetPath(const FString& AssetPath, FString& OutPackagePath, FString& OutAssetName);

	/** Create a FMetasoundFrontendLiteral from a JSON value + data type string */
	static bool CreateLiteralFromJson(
		const TSharedPtr<FJsonValue>& JsonVal, const FString& DataType,
		struct FMetasoundFrontendLiteral& OutLiteral, FString& OutError);

	/** Parse a node class array ["Namespace","Name","Variant"] into FMetasoundFrontendClassName */
	static bool ParseNodeClassName(
		const TArray<TSharedPtr<FJsonValue>>& ClassArray,
		struct FMetasoundFrontendClassName& OutClassName, FString& OutError);

	/** Get or create a builder for an existing MetaSound asset by path */
	static class UMetaSoundBuilderBase* GetBuilderForAsset(const FString& AssetPath, FString& OutError);

	/** Resolve a node_id_or_handle string to a FMetaSoundNodeHandle within a builder.
	 *  Phase F #3: when AssetPath is non-empty, also consults the user-label alias registry
	 *  populated by add_metasound_node so callers can refer to nodes by their assigned label. */
	static bool ResolveNodeHandle(
		class UMetaSoundBuilderBase* Builder, const FString& NodeIdOrHandle,
		struct FMetaSoundNodeHandle& OutHandle, FString& OutError,
		const FString& AssetPath = FString());

	/** Phase F #3: register a user-supplied label -> engine GUID mapping for later resolution. */
	static void RegisterNodeIdAlias(const FString& AssetPath, const FString& UserLabel, const FGuid& NodeGuid);

	/** Phase F #3: look up a user label registered by add_metasound_node. Returns invalid FGuid on miss. */
	static FGuid LookupNodeIdAlias(const FString& AssetPath, const FString& UserLabel);
};

#endif // WITH_METASOUND
