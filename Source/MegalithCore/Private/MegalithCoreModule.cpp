#include "MegalithCoreModule.h"
#include "MegalithHttpServer.h"
#include "MegalithSettings.h"
#include "MegalithJsonUtils.h"
#include "MegalithToolRegistry.h"
#include "MegalithCoreTools.h"
#include "Actions/MegalithBulkFillActions.h"
#include "Misc/FileHelper.h"
#include "GenericPlatform/GenericPlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include "HAL/IConsoleManager.h"

#define LOCTEXT_NAMESPACE "FMegalithCoreModule"

static FAutoConsoleCommand GMegalithRestartCmd(
	TEXT("Megalith.Restart"),
	TEXT("Restart the Megalith MCP HTTP server on its configured port."),
	FConsoleCommandDelegate::CreateStatic(&FMegalithCoreModule::RestartHttpServer)
);

void FMegalithCoreModule::StartupModule()
{
	UE_LOG(LogMegalith, Log, TEXT("Megalith %s — Core module initializing"), MEGALITH_VERSION);

	// Self-heal future-dated mtimes from cross-TZ ZIP extraction.
	NormalizeFutureMtimesIfNeeded();

	// Skip MCP server + sentinel in commandlets (cook/compile). The running editor already holds port 9316
	// and a second bind attempt surfaces as UAT ExitCode=1. Commandlets have no MCP consumer anyway.
	if (IsRunningCommandlet())
	{
		UE_LOG(LogMegalith, Log, TEXT("Megalith — commandlet detected, skipping MCP server startup"));
		return;
	}

	// Register core discovery/status tools
	RegisterCoreTools();

	// Phase 0: register bulk_fill + describe central dispatchers. Per-namespace
	// adapters self-register from their own module's StartupModule via
	// FMegalithBulkFillRegistry::RegisterAdapter — those land in Phases 1-5.
	FMegalithBulkFillActions::RegisterAll();

	// Start HTTP server (gated on bMcpServerEnabled — Issue #38 kill-switch)
	const UMegalithSettings* Settings = UMegalithSettings::Get();
	if (Settings && !Settings->bMcpServerEnabled)
	{
		UE_LOG(LogMegalith, Log,
			TEXT("Megalith — MCP server disabled in settings (bMcpServerEnabled=false), skipping HTTP listener startup"));
		return;
	}

	int32 Port = Settings ? Settings->ServerPort : 9316;

	HttpServer = MakeUnique<FMegalithHttpServer>();
	if (HttpServer->Start(Port))
	{
		WriteSentinelFile(Port);
	}
	else
	{
		UE_LOG(LogMegalith, Error, TEXT("Failed to start MCP server on port %d"), Port);
	}
}

void FMegalithCoreModule::ShutdownModule()
{
	RemoveSentinelFile();

	if (HttpServer.IsValid())
	{
		HttpServer->Stop();
		HttpServer.Reset();
	}

	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("megalith"));
	FMegalithBulkFillActions::UnregisterAll();

	UE_LOG(LogMegalith, Log, TEXT("Megalith — Core module shut down"));
}

void FMegalithCoreModule::RegisterCoreTools()
{
	FMegalithCoreTools::RegisterAll();
}

FString FMegalithCoreModule::GetSentinelFilePath() const
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Megalith"));
	if (Plugin.IsValid())
	{
		return Plugin->GetBaseDir() / TEXT("Saved") / TEXT(".megalith_running");
	}
	return FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("Megalith"), TEXT("Saved"), TEXT(".megalith_running"));
}

void FMegalithCoreModule::WriteSentinelFile(int32 Port)
{
	TSharedPtr<FJsonObject> Sentinel = MakeShared<FJsonObject>();
	Sentinel->SetNumberField(TEXT("pid"), FPlatformProcess::GetCurrentProcessId());
	Sentinel->SetNumberField(TEXT("port"), Port);
	Sentinel->SetStringField(TEXT("version"), MEGALITH_VERSION);
	Sentinel->SetStringField(TEXT("started"), FDateTime::UtcNow().ToIso8601());

	FString Body;
	TSharedRef<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>> Writer =
		TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy<TCHAR>>::Create(&Body);
	FJsonSerializer::Serialize(Sentinel.ToSharedRef(), Writer);

	const FString Path = GetSentinelFilePath();
	if (FFileHelper::SaveStringToFile(Body, *Path, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
	{
		UE_LOG(LogMegalith, Log, TEXT("Sentinel file written: %s"), *Path);
	}
	else
	{
		UE_LOG(LogMegalith, Warning, TEXT("Failed to write sentinel file: %s"), *Path);
	}
}

void FMegalithCoreModule::RemoveSentinelFile()
{
	const FString Path = GetSentinelFilePath();
	if (FPaths::FileExists(Path))
	{
		IFileManager::Get().Delete(*Path);
		UE_LOG(LogMegalith, Log, TEXT("Sentinel file removed: %s"), *Path);
	}
}

void FMegalithCoreModule::NormalizeFutureMtimesIfNeeded()
{
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("Megalith"));
	if (!Plugin.IsValid())
	{
		return;
	}

	const FString PluginDir = Plugin->GetBaseDir();
	const FString UpluginPath = PluginDir / TEXT("Megalith.uplugin");

	const FDateTime UpluginMtime = IFileManager::Get().GetTimeStamp(*UpluginPath);
	if (UpluginMtime == FDateTime::MinValue())
	{
		return;
	}

	const FDateTime NowUtc = FDateTime::UtcNow();
	if (UpluginMtime <= NowUtc)
	{
		return;
	}

	UE_LOG(LogMegalith, Warning, TEXT("Megalith.uplugin mtime %s is in the future (now %s) — normalizing plugin file timestamps"),
		*UpluginMtime.ToIso8601(), *NowUtc.ToIso8601());

	TArray<FString> AllFiles;
	IFileManager::Get().FindFilesRecursive(AllFiles, *PluginDir, TEXT("*"), true, false);

	int32 Touched = 0;
	int32 Failed = 0;
	for (const FString& File : AllFiles)
	{
		if (IFileManager::Get().SetTimeStamp(*File, NowUtc)) { ++Touched; } else { ++Failed; }
	}

	UE_LOG(LogMegalith, Log, TEXT("Normalized %d file(s), %d failed"), Touched, Failed);
}

void FMegalithCoreModule::RestartHttpServer()
{
	if (!IsAvailable())
	{
		UE_LOG(LogMegalith, Warning, TEXT("Megalith.Restart: MegalithCore module not loaded"));
		return;
	}

	FMegalithCoreModule& Module = Get();
	if (!Module.HttpServer.IsValid())
	{
		UE_LOG(LogMegalith, Warning, TEXT("Megalith.Restart: HTTP server instance missing"));
		return;
	}

	const UMegalithSettings* Settings = UMegalithSettings::Get();
	const int32 Port = Settings ? Settings->ServerPort : 9316;

	UE_LOG(LogMegalith, Log, TEXT("Megalith.Restart: restarting HTTP server on port %d"), Port);
	if (Module.HttpServer->Restart(Port))
	{
		Module.WriteSentinelFile(Port);
		UE_LOG(LogMegalith, Log, TEXT("Megalith.Restart: success"));
	}
	else
	{
		UE_LOG(LogMegalith, Error, TEXT("Megalith.Restart: failed to rebind port %d"), Port);
	}
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithCoreModule, MegalithCore)
