#pragma once

#include "Modules/ModuleManager.h"
#include "Logging/LogMacros.h"

/** Log category for the MegalithLevelSequence module (matches MegalithAI / MegalithGAS pattern). */
DECLARE_LOG_CATEGORY_EXTERN(LogMegalithLevelSequence, Log, All);

class FMegalithLevelSequenceModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FDelegateHandle PostEngineInitHandle;
};
