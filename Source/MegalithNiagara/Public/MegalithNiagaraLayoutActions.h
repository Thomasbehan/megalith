#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithNiagaraLayoutActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleAutoLayout(const TSharedPtr<FJsonObject>& Params);
};
