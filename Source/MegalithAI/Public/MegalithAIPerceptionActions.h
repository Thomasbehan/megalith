#pragma once

#include "MegalithAIInternal.h"

class USCS_Node;

class FMegalithAIPerceptionActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// Public helpers (used by anonymous-namespace helper in .cpp)

	/** Find the UAIPerceptionComponent SCS node on a Blueprint */
	static USCS_Node* FindPerceptionNode(UBlueprint* BP);

	/** Find a UAIPerceptionStimuliSourceComponent SCS node */
	static USCS_Node* FindStimuliSourceNode(UBlueprint* BP);

	/** Get the perception component template from an SCS node */
	static class UAIPerceptionComponent* GetPerceptionTemplate(USCS_Node* Node);

private:
	// 109. add_perception_component
	static FMegalithActionResult HandleAddPerceptionComponent(const TSharedPtr<FJsonObject>& Params);

	// 110. get_perception_config
	static FMegalithActionResult HandleGetPerceptionConfig(const TSharedPtr<FJsonObject>& Params);

	// 111. configure_sight_sense
	static FMegalithActionResult HandleConfigureSightSense(const TSharedPtr<FJsonObject>& Params);

	// 112. configure_hearing_sense
	static FMegalithActionResult HandleConfigureHearingSense(const TSharedPtr<FJsonObject>& Params);

	// 113. configure_damage_sense
	static FMegalithActionResult HandleConfigureDamageSense(const TSharedPtr<FJsonObject>& Params);

	// 114. configure_touch_sense
	static FMegalithActionResult HandleConfigureTouchSense(const TSharedPtr<FJsonObject>& Params);

	// 117. remove_sense
	static FMegalithActionResult HandleRemoveSense(const TSharedPtr<FJsonObject>& Params);

	// 118. add_stimuli_source_component
	static FMegalithActionResult HandleAddStimuliSourceComponent(const TSharedPtr<FJsonObject>& Params);

	// 119. configure_stimuli_source
	static FMegalithActionResult HandleConfigureStimuliSource(const TSharedPtr<FJsonObject>& Params);

	// 126. validate_perception_setup
	static FMegalithActionResult HandleValidatePerceptionSetup(const TSharedPtr<FJsonObject>& Params);

	// 218. get_ai_system_config
	static FMegalithActionResult HandleGetAISystemConfig(const TSharedPtr<FJsonObject>& Params);

	// ── Helpers ──

	/** Parse affiliation from JSON (object with enemies/neutrals/friendlies bools, or comma-separated string) */
	static void ParseAffiliation(const TSharedPtr<FJsonObject>& Params, const FString& FieldName,
		bool& bEnemies, bool& bNeutrals, bool& bFriendlies);

	/** Serialize affiliation filter to JSON */
	static TSharedPtr<FJsonObject> AffiliationToJson(const struct FAISenseAffiliationFilter& Filter);

	/** Find or create a sense config of the given class on a perception component template */
	template<typename TSenseConfig>
	static TSenseConfig* FindOrCreateSenseConfig(class UAIPerceptionComponent* PerceptionComp);

	/** Find a sense config of the given class */
	template<typename TSenseConfig>
	static TSenseConfig* FindSenseConfig(class UAIPerceptionComponent* PerceptionComp);

	/** Resolve sense type string to UAISenseConfig subclass */
	static UClass* ResolveSenseConfigClass(const FString& SenseType);

	/** Serialize a single sense config to JSON */
	static TSharedPtr<FJsonObject> SenseConfigToJson(class UAISenseConfig* SenseConfig);
};
