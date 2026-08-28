#include "MegalithAudioModule.h"
#include "MegalithToolRegistry.h"
#include "MegalithSettings.h"
#include "MegalithJsonUtils.h"
#include "MegalithAudioAssetActions.h"
#include "MegalithAudioQueryActions.h"
#include "MegalithAudioBatchActions.h"
#include "MegalithAudioSoundCueActions.h"
#include "MegalithAudioPerceptionActions.h"
#if WITH_METASOUND
#include "MegalithAudioMetaSoundActions.h"
#include "MegalithAudioMetaSoundIntrospectionActions.h"
#endif

// Phase 5 Step 4 (MCP Ergonomics, 2026-05-11) — bulk_fill / describe adapter.
// H5 stub-adapter invariant: Register() ALWAYS runs from StartupModule regardless
// of WITH_METASOUND. M6 invariant: MetaSound paths are #if WITH_METASOUND gated
// INSIDE the adapter; vanilla USoundAttenuation/USoundConcurrency paths run gate-free.
#include "MegalithAudioBulkFillAdapter.h"

void FMegalithAudioModule::StartupModule()
{
	const UMegalithSettings* Settings = GetDefault<UMegalithSettings>();
	if (!Settings || !Settings->bEnableAudio)
	{
		UE_LOG(LogMegalith, Log,
			TEXT("MegalithAudio: Audio module disabled in settings"));
		return;
	}

	FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
	FMegalithAudioAssetActions::RegisterActions(Registry);
	FMegalithAudioQueryActions::RegisterActions(Registry);
	FMegalithAudioBatchActions::RegisterActions(Registry);
	FMegalithAudioSoundCueActions::RegisterActions(Registry);
	FMegalithAudioPerceptionActions::RegisterActions(Registry);
#if WITH_METASOUND
	FMegalithAudioMetaSoundActions::RegisterActions(Registry);
	FMegalithAudioMetaSoundIntrospectionActions::RegisterActions(Registry);
#endif

	// Phase 5 Step 4 — register the audio adapter on the central
	// FMegalithBulkFillRegistry. H5 invariant: this call runs unconditionally;
	// the BODY splits per fill_kind with WITH_METASOUND gating MetaSound paths only.
	FMegalithAudioBulkFillAdapter::Register();

	int32 ActionCount = Registry.GetActions(TEXT("audio")).Num();
	const TCHAR* MetaSoundStatus =
#if WITH_METASOUND
		TEXT("available");
#else
		TEXT("not installed");
#endif
	UE_LOG(LogMegalith, Log, TEXT("MegalithAudio: Loaded (%d actions, MetaSound=%s)"), ActionCount, MetaSoundStatus);
}

void FMegalithAudioModule::ShutdownModule()
{
	FMegalithAudioBulkFillAdapter::Unregister();
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("audio"));
}

IMPLEMENT_MODULE(FMegalithAudioModule, MegalithAudio)
