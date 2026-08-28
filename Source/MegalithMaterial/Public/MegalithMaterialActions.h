#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class UMaterial;
class UMaterialExpression;

/**
 * Material domain action handlers for Megalith.
 * Ported from MaterialMCPReaderLibrary — 14 proven actions.
 */
class FMegalithMaterialActions
{
public:
	/** Register all material actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// --- Action handlers (each takes JSON params, returns FMegalithActionResult) ---
	static FMegalithActionResult GetAllExpressions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetExpressionDetails(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetFullConnectionGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DisconnectExpression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BuildMaterialGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BeginTransaction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult EndTransaction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ExportMaterialGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ImportMaterialGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ValidateMaterial(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RenderPreview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetThumbnail(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateCustomHLSLNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetLayerInfo(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 2: Asset creation & properties ---
	static FMegalithActionResult CreateMaterial(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CreateMaterialInstance(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetMaterialProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DeleteExpression(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 2B: Parameter management, recompile, duplicate ---
	static FMegalithActionResult GetMaterialParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetInstanceParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RecompileMaterial(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DuplicateMaterial(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 2C: Advanced utilities ---
	static FMegalithActionResult GetCompilationStats(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetExpressionProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ConnectExpressions(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 3: Free wins (graph utilities & inspection) ---
	static FMegalithActionResult AutoLayout(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DuplicateExpression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListExpressionClasses(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetExpressionConnections(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult MoveExpression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetMaterialProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 4: Instance & property improvements ---
	static FMegalithActionResult GetInstanceParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetInstanceParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetInstanceParent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ClearInstanceParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SaveMaterial(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 5: Graph editing power ---
	static FMegalithActionResult UpdateCustomHlslNode(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ReplaceExpression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetExpressionPinInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RenameExpression(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ListMaterialInstances(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 6: Material Functions ---
	static FMegalithActionResult CreateMaterialFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BuildFunctionGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetFunctionInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ExportFunctionGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetFunctionMetadata(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult UpdateMaterialFunction(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DeleteFunctionExpression(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 7: Batch & Advanced ---
	static FMegalithActionResult BatchSetMaterialProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult BatchRecompile(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ImportTexture(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 8: Compound workflows ---
	static FMegalithActionResult CreatePbrMaterialFromDisk(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 9: Function instances ---
	static FMegalithActionResult CreateFunctionInstance(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult SetFunctionInstanceParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetFunctionInstanceInfo(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 10: Function utilities ---
	static FMegalithActionResult LayoutFunctionExpressions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult RenameFunctionParameterGroup(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 11: Material expansion ---
	static FMegalithActionResult ClearGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult DeleteExpressions(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GetTextureProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult PreviewTexture(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult PreviewTextures(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult CheckTilingQuality(const TSharedPtr<FJsonObject>& Params);

private:
	/** Load a UMaterial from an asset path. Returns nullptr on failure. */
	static UMaterial* LoadBaseMaterial(const FString& AssetPath);

	/** Serialize a single expression node to JSON. */
	static TSharedPtr<FJsonObject> SerializeExpression(const UMaterialExpression* Expression);

	/**
	 * Shared helper for building expression graphs in both Materials and MaterialFunctions.
	 * Handles node creation (standard + Custom HLSL), property setting, and connection wiring.
	 * Returns nodes_created, connections_made, id_to_name map via the ResultJson out param.
	 * The CreateExpressionFunc callback abstracts the difference between CreateMaterialExpression
	 * and CreateMaterialExpressionInFunction.
	 */
	using FCreateExpressionFunc = TFunction<UMaterialExpression*(UClass* ExprClass, int32 PosX, int32 PosY)>;
	static void BuildGraphFromSpec(
		const TSharedPtr<FJsonObject>& Spec,
		const FCreateExpressionFunc& CreateExpressionFunc,
		TMap<FString, UMaterialExpression*>& IdToExpr,
		int32& OutNodesCreated,
		int32& OutConnectionsMade,
		TArray<TSharedPtr<FJsonValue>>& OutErrors);
};
