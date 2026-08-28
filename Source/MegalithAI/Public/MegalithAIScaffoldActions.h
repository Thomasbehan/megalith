#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIScaffoldActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// 180b. hello_world_ai — ONE-CALL onboarding demo
	static FMegalithActionResult HandleHelloWorldAI(const TSharedPtr<FJsonObject>& Params);

	// 181. scaffold_complete_ai_character — Full AI character stack
	static FMegalithActionResult HandleScaffoldCompleteAICharacter(const TSharedPtr<FJsonObject>& Params);

	// 182. scaffold_perception_to_blackboard — Perception→BB bridge
	static FMegalithActionResult HandleScaffoldPerceptionToBlackboard(const TSharedPtr<FJsonObject>& Params);

	// 184. scaffold_team_system — Team IDs + attitude solver
	static FMegalithActionResult HandleScaffoldTeamSystem(const TSharedPtr<FJsonObject>& Params);

	// 185. scaffold_patrol_investigate_ai — Guard AI
	static FMegalithActionResult HandleScaffoldPatrolInvestigateAI(const TSharedPtr<FJsonObject>& Params);

	// 186. scaffold_enemy_ai — Basic enemy with chase+attack
	static FMegalithActionResult HandleScaffoldEnemyAI(const TSharedPtr<FJsonObject>& Params);

	// 198. scaffold_eqs_move_sequence — RunEQS→MoveTo convenience
	static FMegalithActionResult HandleScaffoldEQSMoveSequence(const TSharedPtr<FJsonObject>& Params);

	// 199. create_bt_from_template — BT templates
	static FMegalithActionResult HandleCreateBTFromTemplate(const TSharedPtr<FJsonObject>& Params);

	// 200. create_st_from_template — ST templates
	static FMegalithActionResult HandleCreateSTFromTemplate(const TSharedPtr<FJsonObject>& Params);

	// 206. batch_validate_ai_assets — Run all validators
	static FMegalithActionResult HandleBatchValidateAIAssets(const TSharedPtr<FJsonObject>& Params);

	// 107. validate_ai_controller — Check controller setup
	static FMegalithActionResult HandleValidateAIController(const TSharedPtr<FJsonObject>& Params);

	// 106. scaffold_ai_controller_blueprint — Full controller setup
	static FMegalithActionResult HandleScaffoldAIControllerBlueprint(const TSharedPtr<FJsonObject>& Params);

	// Genre scaffolds (Task 7)
	static FMegalithActionResult HandleScaffoldCompanionAI(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldBossAI(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldAmbientNPC(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldHorrorStalker(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldHorrorAmbush(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldHorrorPresence(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldHorrorMimic(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldStealthGameAI(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldTurretAI(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldGroupCoordinator(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldFlyingAI(const TSharedPtr<FJsonObject>& Params);

	// ── Internal Helpers ──

	/** Dispatch to a lower-level Megalith action and return the result */
	static FMegalithActionResult Dispatch(const FString& Namespace, const FString& Action, const TSharedPtr<FJsonObject>& Params);

	/** Dispatch and check success, appending error to warnings if it fails */
	static bool DispatchOrWarn(const FString& Namespace, const FString& Action,
		const TSharedPtr<FJsonObject>& Params, TArray<FString>& Warnings, FString StepName);

	/** Build a BT spec JSON for a named template */
	static TSharedPtr<FJsonObject> BuildBTTemplateSpec(const FString& TemplateName, const FString& BBPath);

	/** Build blackboard keys array for a given template */
	static TArray<TSharedPtr<FJsonValue>> BuildBBKeysForTemplate(const FString& TemplateName);
};
