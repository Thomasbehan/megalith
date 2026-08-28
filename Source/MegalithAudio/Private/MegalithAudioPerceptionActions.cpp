#include "MegalithAudioPerceptionActions.h"
#include "MegalithToolRegistry.h"
#include "MegalithParamSchema.h"
#include "MegalithJsonUtils.h"

// Runtime sub-module — owns the UAssetUserData class.
#include "MegalithSoundPerceptionUserData.h"

// Audio asset surface (USoundBase covers SoundCue, SoundWave, MetaSoundSource).
#include "Sound/SoundBase.h"

// Asset registry & save plumbing.
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/ARFilter.h"
#include "UObject/SavePackage.h"
#include "UObject/Package.h"
#include "Misc/PackageName.h"

// AI sense class resolution.
#include "Perception/AISense.h"
#include "Perception/AISense_Hearing.h"

// JSON.
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

namespace
{
	/** Normalize "/Game/Foo/Bar" to "/Game/Foo/Bar.Bar" so FSoftObjectPath resolves cleanly. */
	FString NormalizeAssetPath(const FString& AssetPath)
	{
		FString Out = AssetPath;
		if (!Out.Contains(TEXT(".")))
		{
			int32 LastSlash = INDEX_NONE;
			if (Out.FindLastChar('/', LastSlash) && LastSlash >= 0)
			{
				const FString AssetName = Out.Mid(LastSlash + 1);
				if (!AssetName.IsEmpty())
				{
					Out = Out + TEXT(".") + AssetName;
				}
			}
		}
		return Out;
	}

	/** Registry-first load of a USoundBase, with StaticLoadObject fallback. */
	USoundBase* LoadSoundBase(const FString& AssetPath, FString& OutError)
	{
		const FString Normalized = NormalizeAssetPath(AssetPath);

		IAssetRegistry& Registry = IAssetRegistry::GetChecked();
		FAssetData AssetData = Registry.GetAssetByObjectPath(FSoftObjectPath(Normalized));
		if (AssetData.IsValid())
		{
			UObject* Loaded = AssetData.GetAsset();
			USoundBase* Typed = Cast<USoundBase>(Loaded);
			if (!Typed)
			{
				OutError = FString::Printf(TEXT("Asset at '%s' is not a USoundBase (found %s)"),
					*AssetPath,
					Loaded ? *Loaded->GetClass()->GetName() : TEXT("null"));
			}
			return Typed;
		}

		UObject* Loaded = StaticLoadObject(USoundBase::StaticClass(), nullptr, *AssetPath);
		if (!Loaded)
		{
			OutError = FString::Printf(TEXT("Audio asset not found at '%s'"), *AssetPath);
			return nullptr;
		}
		USoundBase* Typed = Cast<USoundBase>(Loaded);
		if (!Typed)
		{
			OutError = FString::Printf(TEXT("Asset at '%s' is not a USoundBase"), *AssetPath);
		}
		return Typed;
	}

	/** Save the package backing a sound asset to disk. */
	bool SavePackageForAsset(USoundBase* Sound, FString& OutError)
	{
		UPackage* Pkg = Sound ? Sound->GetPackage() : nullptr;
		if (!Pkg)
		{
			OutError = TEXT("Asset has no package");
			return false;
		}
		Pkg->MarkPackageDirty();

		const FString PackageFilename = FPackageName::LongPackageNameToFilename(
			Pkg->GetName(), FPackageName::GetAssetPackageExtension());
		FSavePackageArgs SaveArgs;
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		const bool bSaved = UPackage::SavePackage(Pkg, Sound, *PackageFilename, SaveArgs);
		if (!bSaved)
		{
			OutError = FString::Printf(TEXT("Failed to save package '%s'"), *PackageFilename);
		}
		return bSaved;
	}

	/**
	 * Phase J F11: strict v1 sense_class allowlist parser. Mirrors the F2 ParseOwner idiom —
	 * empty input defaults to Hearing for back-compat; "Hearing" / "AISense_Hearing" (case-insensitive)
	 * are accepted; known-future classes ("Sight", "Damage", "Touch", "Team", "Prediction") return
	 * a distinct deferred-to-v2 error so callers can distinguish capability gaps from typos; everything
	 * else returns the v1-supports enumeration.
	 *
	 * The previous ResolveSenseClass walked TObjectIterator<UClass> matching engine class names
	 * case-insensitively — but "AISense_Sight".Equals("Sight", IgnoreCase) is FALSE, so the walk
	 * silently fell through to the line-127 Hearing default. v1 spec says only Hearing is supported,
	 * so the iterator walk is now dropped entirely.
	 */
	bool ParseSenseClass(const FString& SenseStr, TSubclassOf<UAISense>& OutClass, FString& OutError)
	{
		const FString Trimmed = SenseStr.TrimStartAndEnd();
		if (Trimmed.IsEmpty()
		 || Trimmed.Equals(TEXT("Hearing"), ESearchCase::IgnoreCase)
		 || Trimmed.Equals(TEXT("AISense_Hearing"), ESearchCase::IgnoreCase))
		{
			OutClass = UAISense_Hearing::StaticClass();
			return true;
		}

		// Known-but-deferred future classes — give a specific error so callers know
		// it's a v2 capability gap, not a typo.
		static const TCHAR* DeferredSenses[] = {
			TEXT("Sight"), TEXT("Damage"), TEXT("Touch"), TEXT("Team"), TEXT("Prediction")
		};
		for (const TCHAR* Deferred : DeferredSenses)
		{
			if (Trimmed.Equals(Deferred, ESearchCase::IgnoreCase)
			 || Trimmed.Equals(FString(TEXT("AISense_")) + Deferred, ESearchCase::IgnoreCase))
			{
				OutError = FString::Printf(TEXT("sense_class '%s' deferred to v2"), Deferred);
				return false;
			}
		}

		OutError = FString::Printf(
			TEXT("Unsupported sense_class '%s'. v1 supports: [Hearing]"), *SenseStr);
		return false;
	}

	/**
	 * Phase J F11: pre-flight validator for ApplyParamsToBinding. Mirrors the F2/F3 idiom of
	 * "Parse + Validate, THEN mutate" — runs every numeric/length/membership check before any
	 * write to the UserData struct. Returns false + populates OutError on the first miss.
	 *
	 * Checks (only when the field is present):
	 *   - loudness  >= 0.0    ("loudness must be >= 0")
	 *   - max_range >= 0.0    ("max_range must be >= 0 (use 0 for listener default)")
	 *   - tag.Len() <= 255    ("tag exceeds 255 characters") — project soft-cap, NOT engine FName limit (NAME_SIZE=1024)
	 *   - sense_class via ParseSenseClass strict allowlist
	 */
	bool ValidateBindingParams(const TSharedPtr<FJsonObject>& Params, FString& OutError)
	{
		double DVal = 0.0;
		FString SVal;

		if (Params->TryGetNumberField(TEXT("loudness"), DVal) && DVal < 0.0)
		{
			OutError = TEXT("loudness must be >= 0");
			return false;
		}
		if (Params->TryGetNumberField(TEXT("max_range"), DVal) && DVal < 0.0)
		{
			OutError = TEXT("max_range must be >= 0 (use 0 for listener default)");
			return false;
		}
		if (Params->TryGetStringField(TEXT("tag"), SVal) && SVal.Len() > 255)
		{
			OutError = TEXT("tag exceeds 255 characters");
			return false;
		}
		if (Params->TryGetStringField(TEXT("sense_class"), SVal))
		{
			TSubclassOf<UAISense> ParsedSense;
			FString SenseErr;
			if (!ParseSenseClass(SVal, ParsedSense, SenseErr))
			{
				OutError = SenseErr;
				return false;
			}
		}
		return true;
	}

	/** Build the JSON payload describing a binding (mirrored across bind/get returns). */
	TSharedPtr<FJsonObject> BindingToJson(const UMegalithSoundPerceptionUserData* Data)
	{
		auto Json = MakeShared<FJsonObject>();
		if (!Data)
		{
			return Json;
		}
		Json->SetBoolField(TEXT("enabled"), Data->bEnabled);
		Json->SetNumberField(TEXT("loudness"), Data->Loudness);
		Json->SetNumberField(TEXT("max_range"), Data->MaxRange);
		Json->SetStringField(TEXT("tag"), Data->Tag.ToString());
		Json->SetStringField(TEXT("sense_class"),
			Data->SenseClass ? Data->SenseClass->GetName() : TEXT("AISense_Hearing"));
		Json->SetBoolField(TEXT("fire_on_fade_in"), Data->bFireOnFadeIn);
		Json->SetBoolField(TEXT("require_owning_actor"), Data->bRequireOwningActor);
		return Json;
	}

	/** Apply incoming JSON params onto a UMegalithSoundPerceptionUserData (partial update). */
	void ApplyParamsToBinding(const TSharedPtr<FJsonObject>& Params, UMegalithSoundPerceptionUserData* Data)
	{
		double DVal = 0.0;
		bool BVal = false;
		FString SVal;

		if (Params->TryGetNumberField(TEXT("loudness"), DVal))
		{
			Data->Loudness = static_cast<float>(DVal);
		}
		if (Params->TryGetNumberField(TEXT("max_range"), DVal))
		{
			Data->MaxRange = static_cast<float>(DVal);
		}
		if (Params->TryGetStringField(TEXT("tag"), SVal))
		{
			Data->Tag = SVal.IsEmpty() ? NAME_None : FName(*SVal);
		}
		if (Params->TryGetStringField(TEXT("sense_class"), SVal))
		{
			// Phase J F11: ParseSenseClass is strict — but ValidateBindingParams already ran
			// the same allowlist at the action entry point, so a bad value can't reach here.
			// Discarded OutError is intentional; the validator owns the error path.
			TSubclassOf<UAISense> Parsed;
			FString IgnoredErr;
			if (ParseSenseClass(SVal, Parsed, IgnoredErr))
			{
				Data->SenseClass = Parsed;
			}
		}
		if (Params->TryGetBoolField(TEXT("enabled"), BVal))
		{
			Data->bEnabled = BVal;
		}
		if (Params->TryGetBoolField(TEXT("fire_on_fade_in"), BVal))
		{
			Data->bFireOnFadeIn = BVal;
		}
		if (Params->TryGetBoolField(TEXT("require_owning_actor"), BVal))
		{
			Data->bRequireOwningActor = BVal;
		}
	}
} // namespace

// ============================================================================
// Action: bind_sound_to_perception
// ============================================================================

FMegalithActionResult FMegalithAudioPerceptionActions::BindSoundToPerception(const TSharedPtr<FJsonObject>& Params)
{
	const FString AssetPath = Params->GetStringField(TEXT("asset_path"));
	if (AssetPath.IsEmpty())
	{
		return FMegalithActionResult::Error(TEXT("asset_path is required"));
	}

	// Phase J F11: validate every input field BEFORE loading the asset, BEFORE any mutation.
	// Mirrors the F2/F3 "Parse + Validate, THEN mutate" idiom — silent-coercion paths
	// (negative loudness/max_range, over-length tag, unknown sense_class) now error out cleanly.
	FString ValidateError;
	if (!ValidateBindingParams(Params, ValidateError))
	{
		return FMegalithActionResult::Error(ValidateError);
	}

	FString LoadError;
	USoundBase* Sound = LoadSoundBase(AssetPath, LoadError);
	if (!Sound)
	{
		return FMegalithActionResult::Error(LoadError);
	}

	// Idempotent: reuse an existing binding instead of stacking duplicates.
	UMegalithSoundPerceptionUserData* Data = Cast<UMegalithSoundPerceptionUserData>(
		Sound->GetAssetUserDataOfClass(UMegalithSoundPerceptionUserData::StaticClass()));

	const bool bIsNew = (Data == nullptr);
	if (bIsNew)
	{
		// Outer = the asset itself so the UserData is serialized into the asset's package.
		Data = NewObject<UMegalithSoundPerceptionUserData>(Sound,
			UMegalithSoundPerceptionUserData::StaticClass(), NAME_None, RF_Public);
		Data->SenseClass = UAISense_Hearing::StaticClass();
	}

	Sound->Modify();
	ApplyParamsToBinding(Params, Data);
	if (bIsNew)
	{
		Sound->AddAssetUserData(Data);
	}
	Sound->PostEditChange();

	FString SaveError;
	if (!SavePackageForAsset(Sound, SaveError))
	{
		return FMegalithActionResult::Error(SaveError);
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("asset_path"), Sound->GetPathName());
	Result->SetStringField(TEXT("asset_class"), Sound->GetClass()->GetName());
	Result->SetObjectField(TEXT("binding"), BindingToJson(Data));
	Result->SetBoolField(TEXT("created"), bIsNew);
	Result->SetStringField(TEXT("message"),
		TEXT("Perception binding written to AssetUserData. Runtime listener fires MakeNoise on Playing state when the sound is routed through a UAudioComponent owned by an actor."));
	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// Action: unbind_sound_from_perception
// ============================================================================

FMegalithActionResult FMegalithAudioPerceptionActions::UnbindSoundFromPerception(const TSharedPtr<FJsonObject>& Params)
{
	const FString AssetPath = Params->GetStringField(TEXT("asset_path"));
	if (AssetPath.IsEmpty())
	{
		return FMegalithActionResult::Error(TEXT("asset_path is required"));
	}

	FString LoadError;
	USoundBase* Sound = LoadSoundBase(AssetPath, LoadError);
	if (!Sound)
	{
		return FMegalithActionResult::Error(LoadError);
	}

	const bool bHadBinding = (Sound->GetAssetUserDataOfClass(UMegalithSoundPerceptionUserData::StaticClass()) != nullptr);
	if (!bHadBinding)
	{
		auto Result = MakeShared<FJsonObject>();
		Result->SetStringField(TEXT("asset_path"), Sound->GetPathName());
		Result->SetBoolField(TEXT("removed"), false);
		Result->SetStringField(TEXT("message"), TEXT("No perception binding present on asset"));
		return FMegalithActionResult::Success(Result);
	}

	Sound->Modify();
	Sound->RemoveUserDataOfClass(UMegalithSoundPerceptionUserData::StaticClass());
	Sound->PostEditChange();

	FString SaveError;
	if (!SavePackageForAsset(Sound, SaveError))
	{
		return FMegalithActionResult::Error(SaveError);
	}

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("asset_path"), Sound->GetPathName());
	Result->SetBoolField(TEXT("removed"), true);
	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// Action: get_sound_perception_binding
// ============================================================================

FMegalithActionResult FMegalithAudioPerceptionActions::GetSoundPerceptionBinding(const TSharedPtr<FJsonObject>& Params)
{
	const FString AssetPath = Params->GetStringField(TEXT("asset_path"));
	if (AssetPath.IsEmpty())
	{
		return FMegalithActionResult::Error(TEXT("asset_path is required"));
	}

	FString LoadError;
	USoundBase* Sound = LoadSoundBase(AssetPath, LoadError);
	if (!Sound)
	{
		return FMegalithActionResult::Error(LoadError);
	}

	UMegalithSoundPerceptionUserData* Data = Cast<UMegalithSoundPerceptionUserData>(
		Sound->GetAssetUserDataOfClass(UMegalithSoundPerceptionUserData::StaticClass()));

	auto Result = MakeShared<FJsonObject>();
	Result->SetStringField(TEXT("asset_path"), Sound->GetPathName());
	Result->SetStringField(TEXT("asset_class"), Sound->GetClass()->GetName());
	Result->SetBoolField(TEXT("has_binding"), Data != nullptr);
	if (Data)
	{
		Result->SetObjectField(TEXT("binding"), BindingToJson(Data));
	}
	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// Action: list_perception_bound_sounds
// ============================================================================

FMegalithActionResult FMegalithAudioPerceptionActions::ListPerceptionBoundSounds(const TSharedPtr<FJsonObject>& /*Params*/)
{
	IAssetRegistry& Registry = IAssetRegistry::GetChecked();

	// Filter to USoundBase descendants — registry indexes class hierarchy so this catches Cue, MetaSoundSource, Wave.
	FARFilter Filter;
	Filter.ClassPaths.Add(USoundBase::StaticClass()->GetClassPathName());
	Filter.bRecursiveClasses = true;

	TArray<FAssetData> AllSounds;
	Registry.GetAssets(Filter, AllSounds);

	auto Result = MakeShared<FJsonObject>();
	TArray<TSharedPtr<FJsonValue>> BindingsArr;
	int32 Scanned = 0;
	int32 Bound = 0;

	for (const FAssetData& AssetData : AllSounds)
	{
		++Scanned;

		// AssetUserData is not in registry tags by default — must load to inspect.
		// Cheap when assets are already loaded; cold-cache scans pay the IO. Acceptable for an editor-only list call.
		USoundBase* Sound = Cast<USoundBase>(AssetData.GetAsset());
		if (!Sound)
		{
			continue;
		}

		UMegalithSoundPerceptionUserData* Data = Cast<UMegalithSoundPerceptionUserData>(
			Sound->GetAssetUserDataOfClass(UMegalithSoundPerceptionUserData::StaticClass()));
		if (!Data)
		{
			continue;
		}

		++Bound;
		auto Entry = MakeShared<FJsonObject>();
		Entry->SetStringField(TEXT("asset_path"), Sound->GetPathName());
		Entry->SetStringField(TEXT("asset_class"), Sound->GetClass()->GetName());
		Entry->SetObjectField(TEXT("binding"), BindingToJson(Data));
		BindingsArr.Add(MakeShared<FJsonValueObject>(Entry));
	}

	Result->SetNumberField(TEXT("scanned"), Scanned);
	Result->SetNumberField(TEXT("bound"), Bound);
	Result->SetArrayField(TEXT("bindings"), BindingsArr);
	return FMegalithActionResult::Success(Result);
}

// ============================================================================
// Registration
// ============================================================================

void FMegalithAudioPerceptionActions::RegisterActions(FMegalithToolRegistry& Registry)
{
	Registry.RegisterAction(TEXT("audio"), TEXT("bind_sound_to_perception"),
		TEXT("Stamp a UMegalithSoundPerceptionUserData onto a USoundBase asset (Cue / MetaSoundSource / Wave). Runtime UWorldSubsystem fires AActor::MakeNoise when this sound plays through a UAudioComponent owned by an actor."),
		FMegalithActionHandler::CreateStatic(&FMegalithAudioPerceptionActions::BindSoundToPerception),
		FParamSchemaBuilder()
			.RequiredAssetPath(TEXT("asset_path"), TEXT("Asset path of the USoundBase (e.g. /Game/Audio/SC_Footstep_Heavy)"))
			.Optional(TEXT("loudness"), TEXT("number"), TEXT("FAINoiseEvent::Loudness multiplier (default 1.0)"))
			.Optional(TEXT("max_range"), TEXT("number"), TEXT("Per-event max range in cm; 0 = use listener's HearingRange (default 0)"))
			.Optional(TEXT("tag"), TEXT("string"), TEXT("FName tag for downstream filtering (default empty)"))
			.Optional(TEXT("sense_class"), TEXT("string"), TEXT("Sense class name (only 'Hearing' supported in v1)"))
			.Optional(TEXT("enabled"), TEXT("boolean"), TEXT("Master switch — set false to disable this binding without removing it (default true)"))
			.Optional(TEXT("fire_on_fade_in"), TEXT("boolean"), TEXT("Also fire on FadingIn state, not just Playing (default true)"))
			.Optional(TEXT("require_owning_actor"), TEXT("boolean"), TEXT("Skip 2D / no-owner sounds (default true)"))
			.Build());

	Registry.RegisterAction(TEXT("audio"), TEXT("unbind_sound_from_perception"),
		TEXT("Remove the UMegalithSoundPerceptionUserData entry from a USoundBase asset (no-op if not bound)."),
		FMegalithActionHandler::CreateStatic(&FMegalithAudioPerceptionActions::UnbindSoundFromPerception),
		FParamSchemaBuilder()
			.RequiredAssetPath(TEXT("asset_path"), TEXT("Asset path of the USoundBase"))
			.Build());

	Registry.RegisterAction(TEXT("audio"), TEXT("get_sound_perception_binding"),
		TEXT("Read the current UMegalithSoundPerceptionUserData binding (if any) from a USoundBase asset."),
		FMegalithActionHandler::CreateStatic(&FMegalithAudioPerceptionActions::GetSoundPerceptionBinding),
		FParamSchemaBuilder()
			.RequiredAssetPath(TEXT("asset_path"), TEXT("Asset path of the USoundBase"))
			.Build());

	Registry.RegisterAction(TEXT("audio"), TEXT("list_perception_bound_sounds"),
		TEXT("Scan the AssetRegistry for every USoundBase carrying a Megalith perception binding."),
		FMegalithActionHandler::CreateStatic(&FMegalithAudioPerceptionActions::ListPerceptionBoundSounds),
		FParamSchemaBuilder().Build());
}
