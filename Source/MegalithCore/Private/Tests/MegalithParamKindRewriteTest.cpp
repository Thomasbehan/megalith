// SPDX-License-Identifier: MIT
// Survivor D (Schema-tagged path normalisation) automation tests — plan §12 "Survivor D".
// Plan: Plugins/Megalith/Docs/plans/2026-05-27-mcp-llm-ergonomics.md
//
// DEVIATION NOTE: plan §6 file-table specifies `Source/MegalithCore/Tests/...`.
// This file lives under `Source/MegalithCore/Private/Tests/...` instead so UBT's
// auto-include of `Private/` picks it up without a Build.cs change. Matches the
// existing precedent at `Private/Reflection/Tests/MegalithReflectionWalkerTest.cpp`.
//
// Each test registers a throwaway action under `megalith_test_paths` then
// dispatches via FMegalithToolRegistry::ExecuteAction and inspects the captured
// param values + warnings[] channel. The action handler captures the
// EffectiveParams it actually receives so we can assert the rewrite (or
// non-rewrite) outcome.

#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "MegalithToolRegistry.h"
#include "MegalithParamSchema.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace MegalithParamKindTestDetail
{
	/** A handler that captures whatever path-param value it receives, then returns Success. */
	struct FCapturingHandler
	{
		FString CapturedKey;
		FString CapturedValue;
		bool bSawCall = false;

		FMegalithActionResult operator()(const TSharedPtr<FJsonObject>& Params)
		{
			bSawCall = true;
			if (Params.IsValid())
			{
				Params->TryGetStringField(CapturedKey, CapturedValue);
			}
			TSharedPtr<FJsonObject> R = MakeShared<FJsonObject>();
			R->SetBoolField(TEXT("ok"), true);
			return FMegalithActionResult::Success(R);
		}
	};

	/** Extract warnings[] array as TArray<FString>. Empty if absent. */
	static TArray<FString> GetWarnings(const FMegalithActionResult& R)
	{
		TArray<FString> Out;
		if (!R.Result.IsValid()) return Out;
		const TArray<TSharedPtr<FJsonValue>>* Arr = nullptr;
		if (!R.Result->TryGetArrayField(TEXT("warnings"), Arr) || !Arr) return Out;
		for (const TSharedPtr<FJsonValue>& V : *Arr)
		{
			FString S;
			if (V.IsValid() && V->TryGetString(S))
			{
				Out.Add(S);
			}
		}
		return Out;
	}

	static bool AnyWarningContains(const TArray<FString>& Warnings, const TCHAR* Needle)
	{
		for (const FString& W : Warnings)
		{
			if (W.Contains(Needle)) return true;
		}
		return false;
	}
}

// ---------------------------------------------------------------------------
// Test 1: AssetPath param + backslash payload → rewrite + warning.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMegalithParamKindAssetPathRewriteTest,
	"Megalith.ParamKind.AssetPathRewrite",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMegalithParamKindAssetPathRewriteTest::RunTest(const FString& /*Parameters*/)
{
	using namespace MegalithParamKindTestDetail;

	const FString Ns = TEXT("megalith_test_paths");
	const FString Ac = TEXT("asset_rewrite");
	ON_SCOPE_EXIT { FMegalithToolRegistry::Get().UnregisterNamespace(Ns); };

	auto Cap = MakeShared<FCapturingHandler>();
	Cap->CapturedKey = TEXT("asset_path");

	TSharedPtr<FJsonObject> Schema = FParamSchemaBuilder()
		.RequiredAssetPath(TEXT("asset_path"), TEXT("Tagged AssetPath param under test."))
		.Build();

	FMegalithToolRegistry::Get().RegisterAction(Ns, Ac, TEXT("Test."),
		FMegalithActionHandler::CreateLambda(
			[Cap](const TSharedPtr<FJsonObject>& P) { return (*Cap)(P); }),
		Schema);

	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	Params->SetStringField(TEXT("asset_path"), TEXT("/Game/Foo\\Bar"));

	FMegalithActionResult R = FMegalithToolRegistry::Get().ExecuteAction(Ns, Ac, Params);

	TestTrue(TEXT("call succeeded"), R.bSuccess);
	TestTrue(TEXT("handler was invoked"), Cap->bSawCall);
	TestEqual(TEXT("handler received forward-slash path"), Cap->CapturedValue, FString(TEXT("/Game/Foo/Bar")));
	const TArray<FString> Warnings = GetWarnings(R);
	TestTrue(TEXT("rewrite warning emitted"),
		AnyWarningContains(Warnings, TEXT("Normalised backslashes")));
	return true;
}

// ---------------------------------------------------------------------------
// Test 2: DiskPath param + backslash payload → NO rewrite, NO warning.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMegalithParamKindDiskPathPassThroughTest,
	"Megalith.ParamKind.DiskPathPassThrough",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMegalithParamKindDiskPathPassThroughTest::RunTest(const FString& /*Parameters*/)
{
	using namespace MegalithParamKindTestDetail;

	const FString Ns = TEXT("megalith_test_paths");
	const FString Ac = TEXT("disk_passthrough");
	ON_SCOPE_EXIT { FMegalithToolRegistry::Get().UnregisterNamespace(Ns); };

	auto Cap = MakeShared<FCapturingHandler>();
	Cap->CapturedKey = TEXT("file_path");

	TSharedPtr<FJsonObject> Schema = FParamSchemaBuilder()
		.RequiredDiskPath(TEXT("file_path"), TEXT("Tagged DiskPath param under test."))
		.Build();

	FMegalithToolRegistry::Get().RegisterAction(Ns, Ac, TEXT("Test."),
		FMegalithActionHandler::CreateLambda(
			[Cap](const TSharedPtr<FJsonObject>& P) { return (*Cap)(P); }),
		Schema);

	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	const FString DiskPath = TEXT("C:\\Users\\foo\\bar.cpp");
	Params->SetStringField(TEXT("file_path"), DiskPath);

	FMegalithActionResult R = FMegalithToolRegistry::Get().ExecuteAction(Ns, Ac, Params);

	TestTrue(TEXT("call succeeded"), R.bSuccess);
	TestEqual(TEXT("disk path preserved exactly"), Cap->CapturedValue, DiskPath);
	const TArray<FString> Warnings = GetWarnings(R);
	TestFalse(TEXT("no rewrite warning emitted on DiskPath"),
		AnyWarningContains(Warnings, TEXT("Normalised backslashes")));
	return true;
}

// ---------------------------------------------------------------------------
// Test 3: AssetPath + already canonical → no warning, no spurious work.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMegalithParamKindAssetPathCanonicalNoWarnTest,
	"Megalith.ParamKind.AssetPathCanonicalNoWarn",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMegalithParamKindAssetPathCanonicalNoWarnTest::RunTest(const FString& /*Parameters*/)
{
	using namespace MegalithParamKindTestDetail;

	const FString Ns = TEXT("megalith_test_paths");
	const FString Ac = TEXT("asset_canonical");
	ON_SCOPE_EXIT { FMegalithToolRegistry::Get().UnregisterNamespace(Ns); };

	auto Cap = MakeShared<FCapturingHandler>();
	Cap->CapturedKey = TEXT("asset_path");

	TSharedPtr<FJsonObject> Schema = FParamSchemaBuilder()
		.RequiredAssetPath(TEXT("asset_path"), TEXT("Tagged AssetPath param under test."))
		.Build();

	FMegalithToolRegistry::Get().RegisterAction(Ns, Ac, TEXT("Test."),
		FMegalithActionHandler::CreateLambda(
			[Cap](const TSharedPtr<FJsonObject>& P) { return (*Cap)(P); }),
		Schema);

	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	Params->SetStringField(TEXT("asset_path"), TEXT("/Game/Foo/Bar"));

	FMegalithActionResult R = FMegalithToolRegistry::Get().ExecuteAction(Ns, Ac, Params);

	TestTrue(TEXT("call succeeded"), R.bSuccess);
	TestEqual(TEXT("forward-slash path preserved"), Cap->CapturedValue, FString(TEXT("/Game/Foo/Bar")));
	const TArray<FString> Warnings = GetWarnings(R);
	TestFalse(TEXT("no rewrite warning for canonical path"),
		AnyWarningContains(Warnings, TEXT("Normalised backslashes")));
	return true;
}

// ---------------------------------------------------------------------------
// Test 4: Untagged (Kind == Other) param + backslash payload — legacy
// behaviour preserved. NO rewrite, NO warning. This is the back-compat guard
// for the ~929 existing .Required("asset_path", ...) callsites that have not
// yet been migrated to RequiredAssetPath (Phase 1.1+).
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMegalithParamKindUntaggedLegacyPreservedTest,
	"Megalith.ParamKind.UntaggedLegacyPreserved",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMegalithParamKindUntaggedLegacyPreservedTest::RunTest(const FString& /*Parameters*/)
{
	using namespace MegalithParamKindTestDetail;

	const FString Ns = TEXT("megalith_test_paths");
	const FString Ac = TEXT("untagged_legacy");
	ON_SCOPE_EXIT { FMegalithToolRegistry::Get().UnregisterNamespace(Ns); };

	auto Cap = MakeShared<FCapturingHandler>();
	Cap->CapturedKey = TEXT("some_path");

	// Use the legacy .Required (no kind tag) — Kind == Other by default.
	TSharedPtr<FJsonObject> Schema = FParamSchemaBuilder()
		.Required(TEXT("some_path"), TEXT("string"), TEXT("Untagged legacy param."))
		.Build();

	FMegalithToolRegistry::Get().RegisterAction(Ns, Ac, TEXT("Test."),
		FMegalithActionHandler::CreateLambda(
			[Cap](const TSharedPtr<FJsonObject>& P) { return (*Cap)(P); }),
		Schema);

	const FString InputPath = TEXT("\\contains\\slashes");
	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	Params->SetStringField(TEXT("some_path"), InputPath);

	FMegalithActionResult R = FMegalithToolRegistry::Get().ExecuteAction(Ns, Ac, Params);

	TestTrue(TEXT("call succeeded"), R.bSuccess);
	TestEqual(TEXT("untagged param passed through bit-for-bit"), Cap->CapturedValue, InputPath);
	const TArray<FString> Warnings = GetWarnings(R);
	TestFalse(TEXT("no rewrite warning on untagged param"),
		AnyWarningContains(Warnings, TEXT("Normalised backslashes")));
	return true;
}

// ---------------------------------------------------------------------------
// Test 5: Counter-example regression. Simulate the update_subsystem xcopy
// payload shape — a DiskPath-tagged param carrying native Windows separators.
// Confirms the payload is not corrupted by the AssetPath rewrite.
// ---------------------------------------------------------------------------
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FMegalithParamKindXcopyPayloadIntactTest,
	"Megalith.ParamKind.XcopyPayloadIntact",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FMegalithParamKindXcopyPayloadIntactTest::RunTest(const FString& /*Parameters*/)
{
	using namespace MegalithParamKindTestDetail;

	const FString Ns = TEXT("megalith_test_paths");
	const FString Ac = TEXT("simulate_xcopy");
	ON_SCOPE_EXIT { FMegalithToolRegistry::Get().UnregisterNamespace(Ns); };

	auto Cap = MakeShared<FCapturingHandler>();
	Cap->CapturedKey = TEXT("source_dir");

	TSharedPtr<FJsonObject> Schema = FParamSchemaBuilder()
		.RequiredDiskPath(TEXT("source_dir"), TEXT("DiskPath param mirroring update_subsystem xcopy source."))
		.Build();

	FMegalithToolRegistry::Get().RegisterAction(Ns, Ac, TEXT("Test."),
		FMegalithActionHandler::CreateLambda(
			[Cap](const TSharedPtr<FJsonObject>& P) { return (*Cap)(P); }),
		Schema);

	const FString XcopySrc = TEXT("C:\\Program Files (x86)\\UE_5.7\\Engine\\Source");
	TSharedPtr<FJsonObject> Params = MakeShared<FJsonObject>();
	Params->SetStringField(TEXT("source_dir"), XcopySrc);

	FMegalithActionResult R = FMegalithToolRegistry::Get().ExecuteAction(Ns, Ac, Params);

	TestTrue(TEXT("call succeeded"), R.bSuccess);
	TestEqual(TEXT("xcopy payload preserved verbatim"), Cap->CapturedValue, XcopySrc);
	const TArray<FString> Warnings = GetWarnings(R);
	TestFalse(TEXT("no rewrite warning on DiskPath xcopy payload"),
		AnyWarningContains(Warnings, TEXT("Normalised backslashes")));
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
