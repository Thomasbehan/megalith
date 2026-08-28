#pragma once

#include "Modules/ModuleInterface.h"

class FMegalithGASModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
