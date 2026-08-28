#include "MegalithLevelSequenceModule.h"
#include "MegalithLevelSequenceActions.h"
#include "MegalithLevelSequenceIndexer.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithIndexSubsystem.h"
#include "Editor.h"
#include "Misc/CoreDelegates.h"

#define LOCTEXT_NAMESPACE "FMegalithLevelSequenceModule"

DEFINE_LOG_CATEGORY(LogMegalithLevelSequence);

void FMegalithLevelSequenceModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings) return;

	if (Settings->bEnableLevelSequence)
	{
		FMegalithLevelSequenceActions::RegisterActions(FMegalithToolRegistry::Get());
		const int32 ActionCount = FMegalithToolRegistry::Get().GetActions(TEXT("level_sequence")).Num();
		UE_LOG(LogMegalithLevelSequence, Log, TEXT("MegalithLevelSequence: Loaded (%d actions)"), ActionCount);
	}

	if (Settings->bIndexLevelSequences)
	{
		PostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddLambda([this]()
		{
			if (GEditor)
			{
				if (UMegalithIndexSubsystem* IndexSS = GEditor->GetEditorSubsystem<UMegalithIndexSubsystem>())
				{
					IndexSS->RegisterIndexer(MakeShared<FLevelSequenceIndexer>());
					UE_LOG(LogMegalithLevelSequence, Log, TEXT("MegalithLevelSequence: Registered FLevelSequenceIndexer into MegalithIndex"));
				}
			}
		});
	}
}

void FMegalithLevelSequenceModule::ShutdownModule()
{
	if (PostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(PostEngineInitHandle);
		PostEngineInitHandle.Reset();
	}

	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("level_sequence"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMegalithLevelSequenceModule, MegalithLevelSequence)
