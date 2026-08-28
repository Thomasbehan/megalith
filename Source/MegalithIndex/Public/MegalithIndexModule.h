#pragma once

#include "Modules/ModuleManager.h"

class FMegalithIndexModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
