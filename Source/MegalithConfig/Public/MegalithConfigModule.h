#pragma once

#include "Modules/ModuleManager.h"

class FMegalithConfigModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
