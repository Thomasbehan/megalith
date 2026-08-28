#include "MegalithSettings.h"

UMegalithSettings::UMegalithSettings()
{
}

const UMegalithSettings* UMegalithSettings::Get()
{
	return GetDefault<UMegalithSettings>();
}

TArray<FName> UMegalithSettings::GetIndexedContentPaths()
{
	TArray<FName> Paths;
	Paths.Add(FName(TEXT("/Game")));

	if (const UMegalithSettings* Settings = Get())
	{
		for (const FString& Path : Settings->AdditionalContentPaths)
		{
			if (!Path.IsEmpty())
			{
				Paths.AddUnique(FName(*Path));
			}
		}
	}

	return Paths;
}

bool UMegalithSettings::IsIndexedContentPath(const FString& PackagePath)
{
	if (PackagePath.StartsWith(TEXT("/Game/")))
	{
		return true;
	}

	if (const UMegalithSettings* Settings = Get())
	{
		for (const FString& ContentPath : Settings->AdditionalContentPaths)
		{
			if (!ContentPath.IsEmpty() && PackagePath.StartsWith(ContentPath + TEXT("/")))
			{
				return true;
			}
		}
	}

	return false;
}
