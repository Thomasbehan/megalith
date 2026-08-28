#pragma once

#include "Modules/ModuleInterface.h"

class FMegalithAudioModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
