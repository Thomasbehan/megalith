// Copyright Thomasbehan and Contributors. All Rights Reserved.
// MegalithUISettings.cpp — Phase G

#include "MegalithUISettings.h"

#define LOCTEXT_NAMESPACE "MegalithUISettings"

UMegalithUISettings::UMegalithUISettings()
    : GeneratedStylesPath(TEXT("/Game/UI/Styles/"))
    , CanonicalLibraryPath(TEXT("/Game/UI/Library/"))
    , StyleCacheCap(200)
    , MaxNestingDepth(32)
    , PathCacheCap(256)
{
    // CategoryName is exposed via GetCategoryName() override below — we keep
    // the protected `CategoryName` member untouched so the override is the
    // single source of truth for the section grouping.
}

FName UMegalithUISettings::GetCategoryName() const
{
    // Surfaces under "Plugins" in Project Settings, alongside other
    // Megalith-family settings classes. Matches the long-standing convention
    // used by `UMegalithSettings` (MegalithCore).
    return FName(TEXT("Plugins"));
}

FName UMegalithUISettings::GetSectionName() const
{
    // Section short-name; the editor uses GetSectionText() for the display
    // string. Stable across versions because INI sections key off this name.
    return FName(TEXT("Megalith UI"));
}

#if WITH_EDITOR
FText UMegalithUISettings::GetSectionText() const
{
    return LOCTEXT("MegalithUISettingsSection", "Megalith UI");
}

FText UMegalithUISettings::GetSectionDescription() const
{
    return LOCTEXT(
        "MegalithUISettingsDescription",
        "Configuration for the MegalithUI module: generated-asset paths, cache caps, and validator limits.");
}
#endif

const UMegalithUISettings* UMegalithUISettings::Get()
{
    // GetDefault<> on a UDeveloperSettings is always non-null after the module
    // initialises (the CDO is registered on first access). Returning the
    // const* matches the read-only nature of settings access from the style
    // service hot path.
    return GetDefault<UMegalithUISettings>();
}

FString UMegalithUISettings::NormalizeFolderPath(const FString& Path)
{
    if (Path.IsEmpty())
    {
        return Path;
    }

    FString Out = Path;

    // Strip a trailing slash so callers can always Printf("%s/%s", ...) without
    // worrying about doubling.
    while (Out.Len() > 1 && (Out.EndsWith(TEXT("/")) || Out.EndsWith(TEXT("\\"))))
    {
        Out = Out.LeftChop(1);
    }

    // Ensure a leading slash. Project paths in UE always begin with `/Game/`
    // or `/<Plugin>/`; users sometimes elide the slash in INI entries.
    if (!Out.StartsWith(TEXT("/")))
    {
        Out = FString(TEXT("/")) + Out;
    }

    return Out;
}

#undef LOCTEXT_NAMESPACE
