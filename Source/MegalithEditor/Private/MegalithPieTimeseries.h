#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

// Gap 9: sample_pie_timeseries registration. The action lives under the "animation"
// namespace string (verification ergonomics match sample_pie_anim_instance) but is
// IMPLEMENTED in MegalithEditor — it consumes the editor's async PIE-smoke session
// machinery (FPieSmokeSessionManager) which MegalithAnimation cannot reach. The registry
// is namespace-string-keyed (not module-keyed), so a cross-namespace registration from
// MegalithEditor is well-formed; cleanup rides on MegalithAnimation's
// UnregisterNamespace("animation") (handlers are static, so a lingering registration is
// not a use-after-free of data). The handler delegates to
// FMegalithEditorActions::StartTimeseriesSession (where the PIE-start helpers are in scope).
class FMegalithPieTimeseries
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleSamplePieTimeseries(const TSharedPtr<FJsonObject>& Params);
};
