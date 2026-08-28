#pragma once

#include "Modules/ModuleManager.h"

#if WITH_GEOMETRYSCRIPT
class UMegalithMeshHandlePool;
#endif

class FMegalithMeshModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
#if WITH_GEOMETRYSCRIPT
	UMegalithMeshHandlePool* HandlePool = nullptr;
#endif
};
