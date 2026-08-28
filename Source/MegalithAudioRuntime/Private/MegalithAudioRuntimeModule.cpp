#include "MegalithAudioRuntimeModule.h"

DEFINE_LOG_CATEGORY(LogMegalithAudioRuntime);

void FMegalithAudioRuntimeModule::StartupModule()
{
	UE_LOG(LogMegalithAudioRuntime, Log,
		TEXT("MegalithAudioRuntime: Module loaded (UWorldSubsystem auto-registers per world)"));
}

void FMegalithAudioRuntimeModule::ShutdownModule()
{
	// UWorldSubsystem instances are torn down with their owning UWorld; nothing to do here.
}

IMPLEMENT_MODULE(FMegalithAudioRuntimeModule, MegalithAudioRuntime)
