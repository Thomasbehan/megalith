#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIControllerActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult HandleCreateAIController(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetAIController(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAIControllers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetAIControllerBT(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetPawnAIControllerClass(const TSharedPtr<FJsonObject>& Params);

	// 97. set_ai_controller_flags
	static FMegalithActionResult HandleSetAIControllerFlags(const TSharedPtr<FJsonObject>& Params);

	// 98. set_ai_team
	static FMegalithActionResult HandleSetAITeam(const TSharedPtr<FJsonObject>& Params);

	// 99. get_ai_team
	static FMegalithActionResult HandleGetAITeam(const TSharedPtr<FJsonObject>& Params);

	// 103. spawn_ai_actor
	static FMegalithActionResult HandleSpawnAIActor(const TSharedPtr<FJsonObject>& Params);

	// 104. get_ai_actors
	static FMegalithActionResult HandleGetAIActors(const TSharedPtr<FJsonObject>& Params);
};
