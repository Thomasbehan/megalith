#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"
#include "NiagaraCommon.h"

class UNiagaraSystem;
class UNiagaraNodeOutput;
class UNiagaraNodeFunctionCall;
class UNiagaraScript;
class UNiagaraGraph;
class UNiagaraRendererProperties;
class UNiagaraDataInterface;
struct FVersionedNiagaraEmitterData;
struct FNiagaraVariable;
struct FNiagaraParameterStore;

/**
 * Niagara domain action handlers for Megalith.
 * 89 actions across system, module, parameter, renderer, DI, diagnostics, NPC, effect type, and advanced domains.
 * Waves 1-6 + Phases 3-7.
 * Fixed for UE 5.7 API compatibility.
 */
class FMegalithNiagaraActions
{
public:
	/** Register all niagara actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// --- System (8) ---
	static FMegalithActionResult HandleAddEmitter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveEmitter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateEmitter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEmitterEnabled(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReorderEmitters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEmitterProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRequestCompile(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateSystem(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateStatelessEmitter(const TSharedPtr<FJsonObject>& Params);

	// --- Module (12) ---
	static FMegalithActionResult HandleGetOrderedModules(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleInputs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleGraph(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCustomHLSLText(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCustomHLSLText(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddModule(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveModule(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleMoveModule(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetModuleEnabled(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetModuleInputValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetModuleInputBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetModuleInputDI(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateModuleFromHLSL(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateFunctionFromHLSL(const TSharedPtr<FJsonObject>& Params);

	// --- Parameter (9) ---
	static FMegalithActionResult HandleGetAllParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetUserParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetParameterValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetParameterType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleTraceParameterBinding(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddUserParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveUserParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetParameterDefault(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCurveValue(const TSharedPtr<FJsonObject>& Params);

	// --- Renderer (6) ---
	static FMegalithActionResult HandleAddRenderer(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveRenderer(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRendererMaterial(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRendererProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetRendererBindings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRendererBinding(const TSharedPtr<FJsonObject>& Params);

	// --- Read/Discovery (4) ---
	static FMegalithActionResult HandleListEmitters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListRenderers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListModuleScripts(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListRendererProperties(const TSharedPtr<FJsonObject>& Params);

	// --- Batch (2) ---
	static FMegalithActionResult HandleBatchExecute(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateSystemFromSpec(const TSharedPtr<FJsonObject>& Params);

	// --- Data Interface (1) ---
	static FMegalithActionResult HandleGetDIFunctions(const TSharedPtr<FJsonObject>& Params);

	// --- HLSL (1) ---
	static FMegalithActionResult HandleGetCompiledGPUHLSL(const TSharedPtr<FJsonObject>& Params);

	// --- Diagnostics (1) ---
	static FMegalithActionResult HandleGetSystemDiagnostics(const TSharedPtr<FJsonObject>& Params);

	// --- System Property (2) ---
	static FMegalithActionResult HandleGetSystemProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSystemProperty(const TSharedPtr<FJsonObject>& Params);

	// --- Static Switch (1) ---
	static FMegalithActionResult HandleSetStaticSwitchValue(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 2: Summary & Discovery (4 new) ---
	static FMegalithActionResult HandleGetSystemSummary(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEmitterSummary(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListEmitterProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleInputValue(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 3: DI Curve & Configuration (2 new) ---
	static FMegalithActionResult HandleConfigureCurveKeys(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureDataInterface(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 4: System Management (5 new) ---
	static FMegalithActionResult HandleDuplicateSystem(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetFixedBounds(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEffectType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateEmitter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportSystemSpec(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 5: Dynamic Inputs (3 new) ---
	static FMegalithActionResult HandleAddDynamicInput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetDynamicInputValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchDynamicInputs(const TSharedPtr<FJsonObject>& Params);

	// --- Wave 6: Advanced (3 new) ---
	static FMegalithActionResult HandleAddEventHandler(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateSystem(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSimulationStage(const TSharedPtr<FJsonObject>& Params);

	// --- Composite Helpers (1 new) ---
	static FMegalithActionResult HandleSetSpawnShape(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 3: Dynamic Input Features (5 new) ---
	static FMegalithActionResult HandleListDynamicInputs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDynamicInputTree(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveDynamicInput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDynamicInputValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDynamicInputInputs(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 4: Module & Emitter Management (2 new) ---
	static FMegalithActionResult HandleRenameEmitter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEmitterProperty(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 5: Renderer & DI Improvements (4 new) ---
	static FMegalithActionResult HandleListAvailableRenderers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetRendererMesh(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureRibbon(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureSubUV(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 6A: Event Handlers, Simulation Stages, Module Outputs (7 new) ---
	static FMegalithActionResult HandleGetEventHandlers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEventHandlerProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveEventHandler(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetSimulationStages(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetSimulationStageProperty(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveSimulationStage(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleOutputParameters(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 6B: NPC Support (5 new) ---
	static FMegalithActionResult HandleCreateNPC(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetNPC(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddNPCParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveNPCParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetNPCDefault(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 6B: Effect Type CRUD (3 new) ---
	static FMegalithActionResult HandleCreateEffectType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEffectType(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetEffectTypeProperty(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 6B: Parameter Discovery (1 new) ---
	static FMegalithActionResult HandleGetAvailableParameters(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 6B: Preview (1 new) ---
	static FMegalithActionResult HandlePreviewSystem(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 7: Advanced Features (3 new) ---
	static FMegalithActionResult HandleDiffSystems(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSaveEmitterAsTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCloneModuleOverrides(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 8: Expansion (4 new) ---
	static FMegalithActionResult HandleSaveSystem(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetStaticSwitchValue(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportSystemSpec(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 9: Medium Priority Expansion (5 new) ---
	static FMegalithActionResult HandleGetDIProperties(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleClearEmitterModules(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleScriptInputs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetScalabilitySettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetScalabilitySettings(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListSystems(const TSharedPtr<FJsonObject>& Params);

	// --- Phase 10: Low Priority & QoL (3 new) ---
	static FMegalithActionResult HandleDuplicateModule(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEmitterParent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameUserParameter(const TSharedPtr<FJsonObject>& Params);

	// --- Tranche 2 (#64): read-only Search & Discovery + per-system DI (7 new) ---
	// All asset-registry / per-system traversal, no mutation. Backs the BP wrapper nodes in
	// UMegalithNiagaraQueryLibrary (SearchNiagaraByParameter / ByDataInterface / ByMaterial,
	// QueryNiagara, FindSimilarNiagaraSystems, FindNiagaraReferences, GetNiagaraDataInterfaces).
	static FMegalithActionResult HandleSearchByParameter(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchByDataInterface(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleQueryNiagara(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindSimilarSystems(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchByMaterial(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindNiagaraReferences(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListSystemDataInterfaces(const TSharedPtr<FJsonObject>& Params);

	// --- Helpers (public for use by free functions) ---
	static FString SerializeParameterValue(const FNiagaraVariable& Variable, const FNiagaraParameterStore& Store);

private:
	// --- Internal helpers ---
	static UNiagaraSystem* LoadSystem(const FString& SystemPath);
	static int32 FindEmitterHandleIndex(UNiagaraSystem* System, const FString& HandleIdOrName);
	static bool ResolveScriptUsage(const FString& UsageString, ENiagaraScriptUsage& OutUsage);
	static bool IsSimulationStageUsageString(const FString& UsageString);
	static bool IsParticleEventUsageString(const FString& UsageString);
	static bool ResolveSimulationStageSelector(UNiagaraSystem* System, const FString& EmitterHandleId,
		const TSharedPtr<FJsonObject>& Params, FGuid& OutUsageId, FString* OutStageName = nullptr,
		FString* OutError = nullptr);
	static bool ResolveEventHandlerSelector(UNiagaraSystem* System, const FString& EmitterHandleId,
		const TSharedPtr<FJsonObject>& Params, FGuid& OutUsageId, FString* OutEventName = nullptr,
		FString* OutError = nullptr);
	static FString UsageToString(ENiagaraScriptUsage Usage);
	static UNiagaraGraph* GetGraphForUsage(UNiagaraSystem* System, const FString& EmitterHandleId, ENiagaraScriptUsage Usage);
	static UNiagaraNodeOutput* FindOutputNode(UNiagaraSystem* System, const FString& EmitterHandleId,
		ENiagaraScriptUsage Usage, const FGuid& UsageId = FGuid());
	static UNiagaraNodeFunctionCall* FindModuleNode(UNiagaraSystem* System, const FString& EmitterHandleId,
		const FString& NodeGuidStr, ENiagaraScriptUsage* OutUsage = nullptr, FGuid* OutUsageId = nullptr);
	static UNiagaraNodeFunctionCall* FindFunctionCallNode(UNiagaraSystem* System, const FString& EmitterHandleId, const FString& NodeGuidStr);
	static UClass* ResolveRendererClass(const FString& RendererClass);
	static UNiagaraRendererProperties* GetRenderer(UNiagaraSystem* System, const FString& EmitterHandleId, int32 RendererIndex, FVersionedNiagaraEmitterData** OutEmitterData = nullptr);
	static FNiagaraTypeDefinition ResolveNiagaraType(const FString& TypeName, bool* bOutFellBack = nullptr);
	static FNiagaraVariable MakeUserVariable(const FString& ParamName, const FNiagaraTypeDefinition& TypeDef);
	static FString JsonObjectToString(const TSharedRef<FJsonObject>& JsonObj);
	static FString JsonArrayToString(const TArray<TSharedPtr<FJsonValue>>& JsonArray);
	static FString JsonValueToString(const TSharedPtr<FJsonValue>& Value);
	static UEnum* TryGetStaticSwitchEnum(UEdGraphPin* SwitchPin, UNiagaraNodeFunctionCall* ModuleNode);
	static bool ResolveStaticSwitchEnumValue(UEnum* Enum, const FString& RequestedValue, FString& OutRawValue, FString* OutDisplayValue = nullptr);
	static void AddStaticSwitchEnumMetadata(TSharedRef<FJsonObject> JsonObj, UEnum* Enum, const FString& RawValue);

	// DI override resolution helper — walks override pin upstream to find the DI UObject
	static UNiagaraDataInterface* FindDIFromOverridePin(UNiagaraNodeFunctionCall* ModuleNode, const FName& MatchedFullName, const FNiagaraTypeDefinition& InputType);

	// Shared helper: applies a JSON spec (emitters, user params, renderers, modules) to an existing system.
	// Used by both create_system_from_spec and import_system_spec.
	static int32 ApplySpecToSystem(UNiagaraSystem* System, const FString& SystemPath,
		const TSharedPtr<FJsonObject>& Spec, TArray<FString>& OutErrors);

	// HLSL script creation helper
	static FMegalithActionResult CreateScriptFromHLSL(const TSharedPtr<FJsonObject>& Params, ENiagaraScriptUsage Usage);
};
