#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * Phase 6: Accessibility Analysis Actions (6 actions)
 * Path width validation, navigation complexity, visual contrast,
 * rest point spacing, interactive reach validation, and comprehensive reports.
 * P0 priority — this game serves hospice patients.
 */
class FMegalithMeshAccessibilityActions
{
public:
	/** Register all 6 accessibility actions with the tool registry */
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	static FMegalithActionResult ValidatePathWidth(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ValidateNavigationComplexity(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult AnalyzeVisualContrast(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult FindRestPoints(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult ValidateInteractiveReach(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult GenerateAccessibilityReport(const TSharedPtr<FJsonObject>& Params);
};
