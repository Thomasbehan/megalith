#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverComponentActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Component management (Phase 1 read, Phase 2-3 write)
	static FMegalithActionResult HandleGetSMComponentConfig(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleAddSMComponent(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureSMComponent(const TSharedPtr<FJsonObject>& Params);
};
