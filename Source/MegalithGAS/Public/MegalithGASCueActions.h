#pragma once

#include "MegalithGASInternal.h"

class FMegalithGASCueActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// Phase 2: Cue CRUD
	static FMegalithActionResult HandleCreateGameplayCueNotify(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleLinkCueToEffect(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleUnlinkCueFromEffect(const TSharedPtr<FJsonObject>& Params);
	// Phase 3: Cue Productivity
	static FMegalithActionResult HandleGetCueInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListGameplayCues(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetCueParameters(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleFindCueTriggers(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleValidateCueCoverage(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleBatchCreateCues(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleScaffoldCueLibrary(const TSharedPtr<FJsonObject>& Params);
};
