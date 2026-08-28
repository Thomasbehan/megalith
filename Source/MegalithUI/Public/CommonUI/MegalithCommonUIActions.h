// MegalithCommonUIActions.h
// Public aggregator for the CommonUI action pack. Compiles to empty header when CommonUI absent.
#pragma once

#if WITH_COMMONUI

#include "CoreMinimal.h"

class FMegalithToolRegistry;

class MEGALITHUI_API FMegalithCommonUIActions
{
public:
	/** Register every CommonUI action across all 9 categories with the registry. */
	static void RegisterAll(FMegalithToolRegistry& Registry);
};

#endif // WITH_COMMONUI
