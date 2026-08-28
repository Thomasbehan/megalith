# Megalith — Sibling Plugin Guide

How to extend Megalith with a separate UE plugin that bridges a third-party integration (paid marketplace plugin, license-isolated code, project-specific subsystem) into Megalith's MCP action registry — *without* merging into the core Megalith repo.

**Audience:** plugin authors who want their tool callable as `<namespace>_query({ action, params })` over MCP, but whose plugin can't or shouldn't ship inside Megalith itself.

---

## Why a sibling plugin?

In-tree modules (the ones living in `Plugins/Megalith/Source/Megalith*/`) ship inside every public Megalith release zip. That works for code Megalith owns and licenses permissively, but breaks down for:

| Reason | Example |
|---|---|
| **Paid marketplace dependency** — you can't redistribute someone else's plugin | A bridge that calls into an Epic Marketplace / Fab plugin (Inventory systems, networking kits, AI middleware, etc.) |
| **License isolation** — your code is GPL/MIT/proprietary and the host project is the other | A library wrapper where you need to keep the LICENSE files and source tree separate |
| **Project-specific** — useful for one game, noise for everyone else | A bridge into your game's bespoke save system, gameplay subsystem, or build pipeline |
| **Independent versioning** — your plugin ships on its own cadence | A subsystem that updates faster (or slower) than Megalith's release cycle |
| **Optional binary footprint** — users who don't have the dep shouldn't pay for the DLL | A bridge whose binaries would be dead weight in 90% of installs |

The sibling-plugin pattern lets you do all of this while still appearing as a first-class MCP tool to AI clients (Claude Code, Cursor, Cline) — same registry, same dispatch path, same discovery output.

---

## How it works (conceptually)

```
┌─────────────────────────────────────────────────────────┐
│  Plugins/Megalith/                                      │
│    Source/MegalithCore/                                 │
│      • FMegalithToolRegistry  (MEGALITHCORE_API)        │
│      • HTTP server, MCP dispatch                        │
│    Source/Megalith<Domain>/                             │
│      • In-tree modules: register("namespace", fn)       │
└─────────────────────────────────────────────────────────┘
              ▲
              │  resolves the same
              │  FMegalithToolRegistry::Get() singleton
              │
┌─────────────────────────────────────────────────────────┐
│  Plugins/MyPluginBridge/   ← YOUR sibling plugin         │
│    Source/MyPluginBridge/                                │
│      • StartupModule → register("myns", fn)              │
│      • Build.cs detects third-party dep + #if gates      │
└─────────────────────────────────────────────────────────┘
```

`FMegalithToolRegistry` is a process-wide singleton exported from Megalith's `MEGALITHCORE_API`. Any module — in-tree or sibling — that lists `MegalithCore` as a dependency can call `FMegalithToolRegistry::Get().RegisterHandler("namespace", "action", &MyHandler)` at startup and the action is immediately callable over MCP.

The sibling plugin's namespace shows up under `megalith_discover()` alongside in-tree namespaces. AI clients see no difference.

---

## Prerequisites

Before you start:

1. **Megalith is installed and loading cleanly** in your project. Sibling plugins depend on `Megalith` as a hard plugin dependency.
2. **Your third-party dependency** (if any) is installed and loadable in the same project.
3. **You've claimed a unique namespace** that doesn't collide with Megalith's existing namespaces. Run `megalith_discover()` first — current namespaces are listed in `Plugins/Megalith/Docs/SPEC_CORE.md` §Architecture (e.g. `material`, `mesh`, `niagara`, `inventory`, etc.). Pick something specific to your integration.

---

## Plugin folder layout

Sit beside Megalith, not inside it:

```
<YourProject>/
  Plugins/
    Megalith/                       ← unmodified
      Source/MegalithCore/...
      Megalith.uplugin
    MyPluginBridge/                 ← your sibling plugin
      Source/
        MyPluginBridge/
          MyPluginBridge.Build.cs
          Private/
            MyPluginBridgeModule.cpp
            MyPluginBridgeActions.cpp
          Public/
            MyPluginBridgeModule.h
            MyPluginBridgeActions.h
      MyPluginBridge.uplugin
      Docs/
        SPEC_CORE.md
        specs/
          SPEC_<Subsystem>.md
        ROADMAP.md
        TESTING.md
```

Same docs convention as in-tree Megalith modules (per-module SPEC split for big plugins, single SPEC for small focused ones). See `feedback_local_plugin_docs_convention.md` if you're documenting your bridge.

---

## The .uplugin file

Minimum viable shape:

```json
{
  "FileVersion": 3,
  "Version": 1,
  "VersionName": "0.1.0",
  "FriendlyName": "MyPluginBridge",
  "Description": "Bridges <ThirdPartyPlugin> into the Megalith MCP registry.",
  "Category": "Editor",
  "CreatedBy": "<you>",
  "EnabledByDefault": true,
  "Installed": false,

  "Plugins": [
    { "Name": "Megalith",            "Enabled": true },
    { "Name": "<ThirdPartyPlugin>",  "Enabled": true, "Optional": true }
  ],

  "Modules": [
    {
      "Name": "MyPluginBridge",
      "Type": "Editor",
      "LoadingPhase": "Default"
    }
  ]
}
```

Notes:
- **`Megalith` is a hard plugin dep** — your bridge is meaningless without it.
- **The third-party dep is `Optional: true`** so UE doesn't refuse to load your bridge when the user removes the third-party plugin. Your bridge degrades gracefully (see Build.cs section).
- **`LoadingPhase: Default`** — runs after `PostEngineInit` (when `MegalithCore` finishes initializing the registry). If your bridge needs to register into a subsystem that initializes later, use `PostDefault`.

---

## Build.cs — third-party detection + release-build kill-switch

The canonical pattern is **detect-then-gate**: probe for the third-party plugin at compile time, set a `WITH_<DEP>=0/1` `PublicDefinition`, then gate every dep-using `#include` and call site behind that define. **And** honour `MEGALITH_RELEASE_BUILD=1` so release builds force the dep off regardless of on-disk state.

```csharp
using UnrealBuildTool;
using System.IO;

public class MyPluginBridge : ModuleRules
{
    public MyPluginBridge(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Always-on dependencies (Megalith + UE basics)
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine",
            "MegalithCore"
        });

        // Release builds: set MEGALITH_RELEASE_BUILD=1 to force optional deps off.
        // Public Megalith release zips set this so accidentally-bundled siblings
        // can't end up with a hard import on a paid third-party DLL the user
        // doesn't have installed.
        bool bReleaseBuild =
            System.Environment.GetEnvironmentVariable("MEGALITH_RELEASE_BUILD") == "1";

        bool bHasThirdParty = false;
        if (!bReleaseBuild)
        {
            // Check three locations: project Plugins/, Engine Plugins/Marketplace/, Engine Plugins/
            string ProjectPluginsDir = Path.Combine(
                Target.ProjectFile.Directory.FullName, "Plugins");
            if (Directory.Exists(ProjectPluginsDir))
            {
                bHasThirdParty = Directory.GetDirectories(
                    ProjectPluginsDir, "ThirdParty*",
                    SearchOption.TopDirectoryOnly).Length > 0;
            }

            if (!bHasThirdParty)
            {
                string EngineDir = Path.GetFullPath(Target.RelativeEnginePath);
                string MarketplaceDir = Path.Combine(EngineDir, "Plugins", "Marketplace");
                if (Directory.Exists(MarketplaceDir))
                {
                    bHasThirdParty = Directory.GetDirectories(
                        MarketplaceDir, "ThirdParty*",
                        SearchOption.TopDirectoryOnly).Length > 0;
                }
            }
        }

        if (bHasThirdParty)
        {
            PrivateDependencyModuleNames.Add("ThirdPartyModule");
            PublicDefinitions.Add("WITH_THIRDPARTY=1");
        }
        else
        {
            PublicDefinitions.Add("WITH_THIRDPARTY=0");
        }
    }
}
```

**Why three locations?** Marketplace / Fab installs land in different folders across UE versions and install methods. The wildcard glob (`"ThirdParty*"`) survives obfuscated marketplace folder names.

**Why `MEGALITH_RELEASE_BUILD`?** Without it, anyone building a public Megalith release on a machine that *happens* to have your third-party plugin installed would produce binaries with hard imports on `UnrealEditor-ThirdPartyModule.dll`. Users who download that release and don't have the third-party plugin will hit `LoadLibrary` failures with `GetLastError=126` at module load. The kill-switch is a hard guard against accidental leakage.

This same pattern is used by every in-tree Megalith optional module: `MegalithBABridge` (Blueprint Assist), `MegalithComboGraph`, `MegalithLogicDriver`, `MegalithGAS` (Gameplay Behaviors), `MegalithAudio` (MetaSound), `MegalithAI` (Mass Entity / Zone Graph), `MegalithUI` (CommonUI), `MegalithMesh` (GeometryScripting). Reference any of those Build.cs files for variants.

---

## Module entry — registry registration

```cpp
// MyPluginBridgeModule.cpp
#include "MyPluginBridgeModule.h"
#include "MyPluginBridgeActions.h"
#include "MegalithToolRegistry.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FMyPluginBridgeModule"

void FMyPluginBridgeModule::StartupModule()
{
#if WITH_THIRDPARTY
    FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();
    FMyPluginBridgeActions::RegisterActions(Registry);
    UE_LOG(LogTemp, Log,
        TEXT("MyPluginBridge — registered N actions in 'myns' namespace"));
#else
    UE_LOG(LogTemp, Log,
        TEXT("MyPluginBridge — third-party dep not detected, bridge inactive"));
#endif
}

void FMyPluginBridgeModule::ShutdownModule()
{
    // Always unregister the namespace, even if we never registered handlers.
    // Megalith's registry treats unregister-of-empty as a no-op.
    if (FMegalithToolRegistry::IsAvailable())
    {
        FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("myns"));
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMyPluginBridgeModule, MyPluginBridge)
```

Notes:
- **Always-register the shutdown sweep** even when `WITH_THIRDPARTY=0`. Costs nothing, prevents stale entries surviving hot-reload edge cases.
- **`IsAvailable()` guard on shutdown** — Megalith might be tearing down before you. Defensive.
- **Don't fail StartupModule when the dep is missing.** Log, return cleanly. Your bridge should be invisible (zero registered actions) rather than crashing the editor.

---

## Action handlers — namespace and signatures

Match the Megalith action handler signature so dispatch finds you:

```cpp
// MyPluginBridgeActions.h
#pragma once

#include "CoreMinimal.h"
#include "MegalithActionResult.h"

class FMegalithToolRegistry;

class FMyPluginBridgeActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

#if WITH_THIRDPARTY
    static FMegalithActionResult HandleDoSomething(
        const TSharedPtr<FJsonObject>& Params);
#endif
};
```

```cpp
// MyPluginBridgeActions.cpp
#include "MyPluginBridgeActions.h"
#include "MegalithToolRegistry.h"

#if WITH_THIRDPARTY
#include "ThirdParty/ThirdPartyAPI.h"
#endif

void FMyPluginBridgeActions::RegisterActions(FMegalithToolRegistry& Registry)
{
#if WITH_THIRDPARTY
    Registry.RegisterHandler(
        TEXT("myns"),               // namespace claim
        TEXT("do_something"),       // action name
        &FMyPluginBridgeActions::HandleDoSomething);

    // ... more handlers ...
#endif
}

#if WITH_THIRDPARTY
FMegalithActionResult FMyPluginBridgeActions::HandleDoSomething(
    const TSharedPtr<FJsonObject>& Params)
{
    // Validate params...
    FString TargetName;
    if (!Params->TryGetStringField(TEXT("target"), TargetName))
    {
        return FMegalithActionResult::Error(
            -32602, TEXT("Missing required param: target"));
    }

    // Call into third-party API...
    bool bOk = ThirdPartyAPI::Do(TargetName);

    // Return result as JSON...
    TSharedRef<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetBoolField(TEXT("success"), bOk);
    Result->SetStringField(TEXT("target"), TargetName);
    return FMegalithActionResult::Ok(Result);
}
#endif
```

Conventions:
- **Namespace = lowercase, no spaces, project-specific.** Goes to `<myns>_query` MCP tool.
- **Action name = snake_case.** Verbs preferred (`get_x`, `set_y`, `create_z`).
- **Handler signature is fixed:** `static FMegalithActionResult Handle*(const TSharedPtr<FJsonObject>& Params)`.
- **Errors return JSON-RPC error codes.** `-32601` (method not found), `-32602` (invalid params), `-32603` (internal error). Use `FMegalithActionResult::Error(code, message)`.
- **Successful results return a `TSharedRef<FJsonObject>`** with whatever fields make sense for the action. Wrap in `FMegalithActionResult::Ok(payload)`.

If you have many handler classes, follow the in-tree Megalith convention: one class per logical subsystem (`FMyPluginInspectionActions`, `FMyPluginCRUDActions`, etc.), each with its own `RegisterActions` entry point that the module entry calls in sequence.

---

## Distribution & release discipline

Two distribution scenarios:

### 1. Bridge ships publicly (as a Marketplace / Fab plugin or open-source repo)

- **Independent `.uplugin` versioning** — your bridge has its own VersionName, decoupled from Megalith's.
- **Dependency declaration** — `Plugins[]` lists `Megalith` as a hard dep. Users install both.
- **CI builds on multiple UE versions** if you want to support more than one engine release. Honour the same conditional patterns Megalith uses (`#if ENGINE_MINOR_VERSION >= ...`).
- **Don't redistribute the third-party plugin.** Tell users to install it themselves; your Build.cs detects it.

### 2. Bridge stays private (internal-use, paid-dep wrapper, project-specific)

- **Sibling plugin lives outside the Megalith repo entirely** (separate VCS branch, separate folder). Public Megalith release zip never sees it.
- **Defence in depth:**
  - Public Megalith release script (`Plugins/Megalith/Scripts/make_release.ps1`) gates against accidental sibling inclusion via `git ls-files` (only files tracked by Megalith's own VCS make it into the zip).
  - `MEGALITH_RELEASE_BUILD=1` env-var forces all optional deps off so even an accidentally-bundled sibling can't carry a hard DLL import.
  - Public Megalith docs scrub references to private siblings before push.
- **Per-module specs in your sibling stay private.** Public docs may *acknowledge the existence* of the integration if it's not embarrassing, but the action roster + implementation specs are yours.

---

## Private Examples

Private sibling bridges can target marketplace plugins, project-only systems, or external tooling stacks. Keep those provider names, action rosters, and implementation specs in the sibling repo. Public Megalith should document the pattern, not a downstream project's private bridge inventory.

### Advanced Detection Variant

The basic 3-location probe above (project Plugins/, engine Marketplace/, engine Plugins/) covers the common case. For sibling plugins where the third-party dependency ships at unpredictable folder names or in multiple legitimate engine locations, use a helper-struct pattern:

```csharp
private sealed class VendorPluginDetection
{
    public bool bFound;
    public string Root = string.Empty;
    public string Source = "none";
}

private static VendorPluginDetection DetectVendorPlugin(ReadOnlyTargetRules Target, bool bReleaseBuild)
{
    if (bReleaseBuild) return new VendorPluginDetection();

    static bool HasUplugin(string root) =>
        Directory.Exists(root) && File.Exists(Path.Combine(root, "VendorPlugin.uplugin"));

    VendorPluginDetection Found(string root, string source) =>
        new VendorPluginDetection { bFound = true, Root = root, Source = source };

    // 1. Project Plugins/Runtime/VendorPlugin (canonical install location)
    if (Target.ProjectFile != null)
    {
        string projectRuntimeRoot = Path.Combine(
            Target.ProjectFile.Directory.FullName, "Plugins", "Runtime", "VendorPlugin");
        if (HasUplugin(projectRuntimeRoot)) return Found(projectRuntimeRoot, "project_runtime");

        string projectRoot = Path.Combine(
            Target.ProjectFile.Directory.FullName, "Plugins", "VendorPlugin");
        if (HasUplugin(projectRoot)) return Found(projectRoot, "project_root");
    }

    // 2. Engine Plugins/Marketplace/ — recursive scan for ANY VendorPlugin.uplugin
    //    handles Fab/Marketplace folder name obfuscation
    string engineDir = Path.GetFullPath(Target.RelativeEnginePath);
    string marketplaceDir = Path.Combine(engineDir, "Plugins", "Marketplace");
    if (Directory.Exists(marketplaceDir))
    {
        string[] matches = Directory.GetFiles(
            marketplaceDir, "VendorPlugin.uplugin", SearchOption.AllDirectories);
        string firstMatch = matches.OrderBy(p => p, StringComparer.OrdinalIgnoreCase).FirstOrDefault();
        if (!string.IsNullOrEmpty(firstMatch))
            return Found(Path.GetDirectoryName(firstMatch), "engine_marketplace");
    }

    // 3. Engine Plugins/Runtime/VendorPlugin (stock-engine fallback)
    string engineRuntimeRoot = Path.Combine(engineDir, "Plugins", "Runtime", "VendorPlugin");
    if (HasUplugin(engineRuntimeRoot)) return Found(engineRuntimeRoot, "engine_runtime");

    return new VendorPluginDetection();
}
```

Then in the constructor:
```csharp
VendorPluginDetection Detection = DetectVendorPlugin(Target, bReleaseBuild);
if (Detection.bFound)
{
    PrivateDependencyModuleNames.AddRange(new[] { "VendorRuntime", "VendorEditor" });
    PublicDefinitions.Add("WITH_VENDORPLUGIN=1");
    Console.WriteLine($"MegalithVendorBridge: detected vendor plugin at {Detection.Root} ({Detection.Source})");
}
else
{
    PublicDefinitions.Add("WITH_VENDORPLUGIN=0");
    Console.WriteLine("MegalithVendorBridge: vendor plugin not detected or release stub forced; compiling stub mode");
}
```

**Why this is better than the basic 3-location probe:**
1. **Helper struct captures *where* the dep was found** — invaluable when debugging "WITH_X is unexpectedly 0/1 in this build" issues at UBT time
2. **`HasUplugin` predicate** avoids false positives where the folder exists but the plugin doesn't
3. **Recursive Marketplace scan** survives the random folder names Fab/Marketplace generates
4. **Console.WriteLine at detection** surfaces the result in UBT output without needing to set verbose logging
5. **Always guards `Target.ProjectFile != null`** — engine-only / Program targets can have null ProjectFile and would otherwise NRE

Use the basic 3-location probe for simple cases. Use the helper-struct + recursive-scan pattern when the third-party plugin's install location is unpredictable or when you want richer UBT-time observability.

The in-tree optional modules (`MegalithBABridge`, `MegalithComboGraph`, `MegalithLogicDriver`, `MegalithGAS`, `MegalithAudio`, `MegalithAI`, `MegalithUI`, `MegalithMesh`) all use the same conditional-compilation pattern described in the Build.cs section above — those are the canonical reference implementations. Crack any of them open for variants of the detection probe and `RegisterActions` shape.

---

## Testing your sibling plugin

1. **Compile both with and without the third-party dep installed.**
   - With dep present: `WITH_THIRDPARTY=1`, all your actions register, full functionality.
   - With dep absent: `WITH_THIRDPARTY=0`, your module compiles clean, no actions register, log line states "bridge inactive".
2. **Compile with `MEGALITH_RELEASE_BUILD=1` set.** Should behave identically to "dep absent" — no hard DLL import, zero registered actions. Verify with `dumpbin /imports Binaries/Win64/UnrealEditor-MyPluginBridge.dll` (Windows) or `otool -L` (macOS).
3. **`megalith_discover()`** should list your namespace in the response when `WITH_THIRDPARTY=1`, and *not* list it when `=0`.
4. **Round-trip an MCP call:**
   ```
   curl -sS -X POST http://localhost:9316/mcp \
     -H "Content-Type: application/json" \
     -d '{"jsonrpc":"2.0","id":1,"method":"tools/call",
          "params":{"name":"myns_query",
                    "arguments":{"action":"do_something",
                                 "params":{"target":"foo"}}}}'
   ```
5. **Hot-reload survival.** Edit a handler, recompile via Live Coding, re-run the MCP call. The handler should execute the new code without an editor restart. (Live Coding patches are in-memory only; for header changes, full editor restart + UBT.)

---

## Common pitfalls

| Pitfall | Symptom | Fix |
|---|---|---|
| **Forgot `MEGALITH_RELEASE_BUILD` honor** | Released DLL hard-imports a third-party DLL the user doesn't have → `GetLastError=126` at module load | Add the env-var check before `Directory.Exists` (see Build.cs section) |
| **Namespace collision** | Your handlers don't fire; Megalith's existing handlers fire instead | Run `megalith_discover()` before claiming a namespace; pick something specific |
| **Module loads before MegalithCore** | `FMegalithToolRegistry::Get()` returns null at startup | `LoadingPhase: Default` (not `PreEngineInit` / `PostConfigInit`) |
| **Forgot `IsAvailable()` guard on shutdown** | Editor exit crash on plugin unload during teardown | Wrap registry calls in `if (FMegalithToolRegistry::IsAvailable())` in `ShutdownModule` |
| **Registered actions outside `#if WITH_THIRDPARTY`** | Actions register but every call returns -32601 because handler bodies are stubbed out | Move both `RegisterHandler` calls AND the handler bodies inside the `#if` |
| **Handler doesn't return JSON-RPC error on missing param** | AI client gets cryptic 500 instead of actionable error message | Validate params at the top, return `FMegalithActionResult::Error(-32602, ...)` |

---

## See also

- [`SPEC_CORE.md`](SPEC_CORE.md) — Megalith plugin overview, registered namespaces, action counts
- [`Plugins/Megalith/Source/MegalithCore/Public/MegalithToolRegistry.h`](../Source/MegalithCore/Public/MegalithToolRegistry.h) — registry API surface (RegisterHandler, UnregisterNamespace, Get, IsAvailable)
- [`Plugins/Megalith/Source/MegalithBABridge/MegalithBABridge.Build.cs`](../Source/MegalithBABridge/MegalithBABridge.Build.cs) — canonical detection-probe + `MEGALITH_RELEASE_BUILD` Build.cs pattern
- [`Plugins/Megalith/Scripts/make_release.ps1`](../Scripts/make_release.ps1) — public release pipeline; understand its dirty-tree gate, `MEGALITH_RELEASE_BUILD=1` injection, and `$StrippedModules` belt-and-braces strip list
