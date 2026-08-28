#pragma once

#include "Modules/ModuleManager.h"

class FMegalithBlueprintModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
