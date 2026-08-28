#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"
#include "Misc/OutputDevice.h"
#include "Components/SceneCaptureComponent2D.h" // ESceneCaptureSource

struct FMegalithLogEntry
{
	double Timestamp;
	FName Category;
	ELogVerbosity::Type Verbosity;
	FString Message;
};

class FMegalithLogCapture : public FOutputDevice
{
public:
	static constexpr int32 MaxEntries = 10000;

	void Serialize(const TCHAR* V, ELogVerbosity::Type Verbosity, const FName& Category) override;

	TArray<FMegalithLogEntry> GetRecentEntries(int32 Count) const;
	TArray<FMegalithLogEntry> SearchEntries(const FString& Pattern, const FString& CategoryFilter, ELogVerbosity::Type MaxVerbosity, int32 Limit) const;
	TArray<FMegalithLogEntry> GetEntriesSince(double SinceTimestamp, const TArray<FName>& CategoryFilter, ELogVerbosity::Type MaxVerbosity, int32 Limit) const;
	TArray<FString> GetActiveCategories() const;

	int32 GetCountByVerbosity(ELogVerbosity::Type Verbosity) const;
	int32 GetTotalCount() const;
	int32 CountErrorsSince(double SinceTimestamp) const;

private:
	mutable FCriticalSection Lock;
	TArray<FMegalithLogEntry> RingBuffer;
	int32 WriteIndex = 0;
	bool bWrapped = false;

	int32 TotalFatal = 0;
	int32 TotalError = 0;
	int32 TotalWarning = 0;
	int32 TotalLog = 0;
	int32 TotalVerbose = 0;
};

class FMegalithEditorActions
{
public:
	static void RegisterActions(FMegalithLogCapture* LogCapture);
	static void InitLiveCodingDelegate();

	static FMegalithActionResult HandleTriggerBuild(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBuildErrors(const TSharedPtr<FJsonObject>& Params);

	/**
	 * Deterministic error->fix-hint pattern table (item 8). Given the already-built
	 * error-object array, returns an ADDITIVE array of {error_index, pattern, hint}
	 * objects (one per matched error) AND stamps a `fix_hint` string onto the matched
	 * error objects in-place. Existing error fields are never mutated otherwise.
	 * Reads the borrowed source DB (FScopeLock on its lock) for LNK2019 owner-module
	 * resolution + the C4996 deprecation message; degrades to a generic hint when the
	 * DB is unavailable. Public for unit testing (FixHintsAdditive).
	 */
	static TArray<TSharedPtr<FJsonValue>> BuildFixHints(const TArray<TSharedPtr<FJsonValue>>& ErrorObjs);
	static FMegalithActionResult HandleGetBuildStatus(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetBuildSummary(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchBuildOutput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCompileOutput(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetRecentLogs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSearchLogs(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleTailLog(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetLogCategories(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetLogStats(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetCrashContext(const TSharedPtr<FJsonObject>& Params);

	// --- Capture actions ---
	static FMegalithActionResult HandleCaptureScenePreview(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCaptureSequenceFrames(const TSharedPtr<FJsonObject>& Params);
	// #15 preview + capture a skeletal animation asset (AnimSequence / BlendSpace /
	// AnimBlueprint) to PNG frames at requested time samples. Reuses the same
	// FAdvancedPreviewScene -> USceneCaptureComponent2D -> RenderAndSaveCapture path.
	static FMegalithActionResult HandleCaptureAnimFrames(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCaptureSystemGif(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportTexture(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetViewportInfo(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleStitchFlipbook(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteAssets(const TSharedPtr<FJsonObject>& Params);

	// --- Inspect actions (Phase 2: 2026-05-26-megalith-editor-preview-expansion plan) ---
	// Structured-data introspection — no render path. Bodies live in
	// MegalithEditorInspectActions.cpp.
	static FMegalithActionResult HandleInspectMaterialPBR(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleInspectTextureChannels(const TSharedPtr<FJsonObject>& Params);

	// --- Composite-capture actions (Phase 3: 2026-05-26-megalith-editor-preview-expansion plan) ---
	// Multi-asset / show-flag overlay capture. Bodies live in
	// MegalithEditorPreviewActions.cpp.
	static FMegalithActionResult HandleCaptureMaterialGrid(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCaptureWithOverlay(const TSharedPtr<FJsonObject>& Params);

	// --- Automation tests ---
	static FMegalithActionResult HandleRunAutomationTests(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListAutomationTests(const TSharedPtr<FJsonObject>& Params);

	// --- Scripting actions (HOFF 7) ---
	static FMegalithActionResult HandleRunPython(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleLoadLevel(const TSharedPtr<FJsonObject>& Params);

	// --- Runtime / PIE control ---
	// Execute a console command in the active PIE world. Lets external tooling
	// drive in-game testing (toggle exec cmds, view modes, debug commands).
	static FMegalithActionResult HandleRunConsoleCommand(const TSharedPtr<FJsonObject>& Params);

	// Start a Play-In-Editor session. Equivalent to pressing Cmd/Ctrl+P in the
	// editor. Returns immediately after queuing the session — actual world spawn
	// happens on the next editor tick.
	static FMegalithActionResult HandleStartPIE(const TSharedPtr<FJsonObject>& Params);

	// Stop the active Play-In-Editor session.
	static FMegalithActionResult HandleStopPIE(const TSharedPtr<FJsonObject>& Params);

	// --- Package state (F1: PIE/profiling harness plan 2026-06-04) ---
	// Scoped dirty-package report + scoped saver with fail-on-unrequested-dirty.
	static FMegalithActionResult HandleListDirtyPackages(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSavePackages(const TSharedPtr<FJsonObject>& Params);

	// --- Async session-based PIE smoke (F2/F3: PIE/profiling harness plan 2026-06-04) ---
	// run_pie_smoke starts PIE + registers a session and RETURNS IMMEDIATELY; the
	// editor's real frame loop advances the session via the shared frame observer
	// (FPieSmokeSessionManager). poll_pie_smoke reads progress / the final report;
	// stop_pie_smoke forces RequestEndPlayMap + finalises. capture_pie_movement_clip
	// uses the same session model plus per-interval viewport frame capture.
	static FMegalithActionResult HandleRunPieSmoke(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandlePollPieSmoke(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleStopPieSmoke(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCapturePieMovementClip(const TSharedPtr<FJsonObject>& Params);

	// Gap 9: start a time-series PIE session (same async lifecycle as run_pie_smoke:
	// returns {session_id, status:'running'}; polled via poll_pie_smoke, stopped via
	// stop_pie_smoke). Lives here so it reuses the in-TU PIE-start + map-load + compile-gate
	// helpers and the shared FPieSmokeSessionManager. Registered under the "animation"
	// namespace from the editor module; the handler delegates here.
	static FMegalithActionResult StartTimeseriesSession(const TSharedPtr<FJsonObject>& Params);

	// Read-only scan of loaded UBlueprints for the engine's PIE compile-error
	// condition (BS_Error && bDisplayCompilePIEWarning). Returns {count, blueprints:[{name, path}]}.
	// Same scan run_pie_smoke's on_compile_errors=refuse gate uses to avoid starting
	// PIE on a broken world (which would raise a game-thread-blocking modal).
	static FMegalithActionResult HandleListErroredBlueprints(const TSharedPtr<FJsonObject>& Params);

	// --- Nav harness map builder (F4: PIE/profiling harness plan 2026-06-04) ---
	// Build a test map from a JSON spec (floor, nav bounds, camera, target points,
	// actor instances), rebuild + validate nav via runtime `ai` dispatch, save.
	static FMegalithActionResult HandleCreateNavHarnessMap(const TSharedPtr<FJsonObject>& Params);

	// --- Generic map settings authoring (Phase 10 / OG-E4, plan 2026-06-07) ---
	// Set WorldSettings GameMode override + spawn APlayerStart actors (+ optional generic
	// actor instances with reflective UPROPERTY defaults) on the open / specified map.
	static FMegalithActionResult HandleAuthorMapSettings(const TSharedPtr<FJsonObject>& Params);

	static void OnLiveCodingPatchComplete();

private:
	static FMegalithLogCapture* CachedLogCapture;

	static double LastCompileTimestamp;
	static FString LastCompileResult;
	static bool bIsCompiling;
	static bool bPatchApplied;
	static double LastCompileEndTimestamp;

	// Capture helpers
	static bool CaptureNiagaraFrame(
		class UNiagaraSystem* System, float SeekTime,
		const FVector& CameraLocation, const FRotator& CameraRotation, float FOV,
		int32 ResX, int32 ResY, const FString& OutputPath,
		ESceneCaptureSource CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR);

	static bool CaptureMaterialFrame(
		class UMaterialInterface* Material, const FString& MeshType,
		const FVector& CameraLocation, const FRotator& CameraRotation, float FOV,
		int32 ResX, int32 ResY, const FString& OutputPath,
		ESceneCaptureSource CaptureSource = ESceneCaptureSource::SCS_FinalToneCurveHDR,
		float UVTiling = 1.0f,
		const FLinearColor& BackgroundColor = FLinearColor(0.18f, 0.18f, 0.18f));

	static bool RenderAndSaveCapture(
		class USceneCaptureComponent2D* CaptureComp,
		class UTextureRenderTarget2D* RT,
		int32 ResX, int32 ResY, const FString& OutputPath);

	// --- PIE-smoke helpers (F2/F3) ---
	// Queue a PIE session pinned to the active level viewport. Returns false (with
	// OutError set) when no viewport / GUnrealEd is available, or when PIE is
	// already running. The session is async/queued; callers must pump editor ticks
	// via PumpEditorUntilPieReady before probing the world.
	// When bSuppressModals is true, the PIE request is wrapped in a GIsRunningUnattendedScript
	// guard so the engine's blocking compile-error prompt resolves to its default
	// instead of starving the game-thread MCP server (used by on_compile_errors=suppress).
	static bool StartPieInternal(FString& OutError, bool bSuppressModals = false);

	// Request the active PIE session to end. Returns true if a session was running.
	static bool StopPieInternal();

	// Find the active PIE world context's UWorld, or nullptr when no PIE is running.
	// Public: the anonymous-namespace map-load guard (EnsureNoResidentPieWorldBeforeMapLoad)
	// and lifecycle reporting read this read-only PIE-residency probe from free-function scope.
public:
	static class UWorld* FindActivePieWorld();
};
