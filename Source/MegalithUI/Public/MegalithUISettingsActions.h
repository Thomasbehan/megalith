// MegalithUISettingsActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUISettingsActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleScaffoldGameUserSettings(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleScaffoldSaveGame(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleScaffoldSaveSubsystem(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleScaffoldAudioSettings(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleScaffoldInputRemapping(const TSharedPtr<FJsonObject>& Params);

private:
    /** Resolve the Source/<ModuleName>/ directory, creating it if needed. Returns empty string on failure. */
    static FString ResolveSourceDir(const FString& ModuleName, FMegalithActionResult& OutError);

    /** Write a .h and .cpp pair. Returns true on success. */
    static bool WriteSourceFiles(const FString& Dir, const FString& ClassName,
        const FString& HeaderContent, const FString& CppContent,
        TSharedPtr<FJsonObject>& OutResult, FMegalithActionResult& OutError);
};
