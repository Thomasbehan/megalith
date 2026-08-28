#pragma once

#include "Modules/ModuleManager.h"

class FMegalithSourceModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
