#pragma once

#include "Modules/ModuleManager.h"

#define MEGALITH_VERSION TEXT("0.22.0")

class FMegalithHttpServer;

class MEGALITHCORE_API FMegalithCoreModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FMegalithCoreModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FMegalithCoreModule>("MegalithCore");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("MegalithCore");
	}

	/** Get the running HTTP server instance */
	FMegalithHttpServer* GetHttpServer() const { return HttpServer.Get(); }

	/** Console-command target: stop and restart the HTTP server on its configured port. */
	static void RestartHttpServer();

private:
	TUniquePtr<FMegalithHttpServer> HttpServer;

	void RegisterCoreTools();
	void WriteSentinelFile(int32 Port);
	void RemoveSentinelFile();
	FString GetSentinelFilePath() const;

	/** Touch plugin files if Megalith.uplugin shows a future mtime (cross-TZ ZIP extraction artifact). */
	void NormalizeFutureMtimesIfNeeded();
};
