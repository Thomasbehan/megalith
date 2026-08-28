#pragma once

#include "Modules/ModuleManager.h"

class FMegalithAnimationModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
