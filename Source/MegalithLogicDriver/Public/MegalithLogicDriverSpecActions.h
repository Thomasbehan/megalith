#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithLogicDriverSpecActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Import/Export/Spec (Phase 2-3)
	static FMegalithActionResult HandleExportSMJson(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportSMJson(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBuildSMFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleExportSMSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCompareStateMachines(const TSharedPtr<FJsonObject>& Params);
};
