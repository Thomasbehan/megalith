#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithBlueprintTemplateActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleApplyTemplate(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListTemplates(const TSharedPtr<FJsonObject>& Params);
};
