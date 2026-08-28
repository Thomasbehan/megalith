#pragma once

#include "Modules/ModuleManager.h"

class FMegalithMaterialModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
