#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverScaffoldActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Scaffold presets (Phase 2-4)
	static FMegalithActionResult HandleScaffoldHelloWorldSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldDialogueSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldQuestSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldInteractableSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldWeaponSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldHorrorEncounterSM(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldGameFlowSM(const TSharedPtr<FJsonObject>& Params);
};
