#pragma once

#include "Modules/ModuleManager.h"

class FMegalithUIModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
