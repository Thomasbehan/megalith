#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverTextGraphActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Text graph inspection (Phase 3)
	static FMegalithActionResult HandleGetTextGraphContent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetDialogueFlow(const TSharedPtr<FJsonObject>& Params);
};
