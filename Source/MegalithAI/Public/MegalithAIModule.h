#pragma once

#include "Modules/ModuleInterface.h"

class FMegalithAIModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FDelegateHandle PostEngineInitHandle;
};
