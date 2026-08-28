# IModularFeatures Bridge Pattern

**Plugin:** Megalith | **Engine:** UE 5.7 | **Validated:** MegalithBABridge (2026-03-27)

---

## Purpose

When a **core** Megalith module (e.g., `MegalithBlueprint`) needs to optionally call into a
third-party C++ API, it cannot take a compile-time dependency on that API — users who don't
have the third party plugin installed would fail to build.

The IModularFeatures bridge solves this:

- The abstract interface lives in `MegalithCore` (always compiled in)
- The implementation lives in a bridge module that **does** depend on the third party
- The core module calls `IsAvailable()` at runtime before touching the interface

Zero compile-time coupling. The bridge module is absent if the third party isn't installed.

---

## When to Use This vs FMegalithToolRegistry

These are complementary patterns. Choose based on who the caller is.

| Scenario | Pattern |
|----------|---------|
| AI agent calls optional MCP actions (`gba_query`, `combograph_query`) | `FMegalithToolRegistry` — see `OPTIONAL_MODULE_ARCHITECTURE.md` |
| Core C++ calls optional C++ API at action time | **IModularFeatures bridge (this doc)** |
| Both — optional module owns MCP actions AND core needs its C++ API | Both patterns simultaneously |

**Rule of thumb:** If the consumer is a JSON MCP action dispatching to another namespace, use
the registry. If the consumer is C++ inside a core module calling a typed API, use
IModularFeatures.

---

## Existing Implementation: IMegalithGraphFormatter

The only current bridge. Enables `MegalithBlueprint`'s `auto_layout` action to delegate to
Blueprint Assist's formatter when BA is installed.

### Files

| Role | File |
|------|------|
| Abstract interface | `Source/MegalithCore/Public/IMegalithGraphFormatter.h` |
| Bridge module | `Source/MegalithBABridge/Private/MegalithBABridgeModule.cpp` |
| Implementation | `Source/MegalithBABridge/Private/MegalithBAFormatterImpl.h/.cpp` |
| Consumer | `Source/MegalithBlueprint/Private/MegalithBlueprintLayoutActions.cpp` |

### Interface (MegalithCore)

```cpp
// IMegalithGraphFormatter.h — in MegalithCore, no BA dependency
class IMegalithGraphFormatter : public IModularFeature
{
public:
    static FName GetModularFeatureName()
    {
        static const FName Name(TEXT("MegalithGraphFormatter"));
        return Name;
    }

    virtual bool SupportsGraph(UEdGraph* Graph) const = 0;
    virtual bool FormatGraph(UEdGraph* Graph, int32& OutNodesFormatted, FString& OutErrorMessage) = 0;
    virtual FMegalithFormatterInfo GetFormatterInfo(UEdGraph* Graph) const = 0;

    static bool IsAvailable()
    {
        return IModularFeatures::Get().IsModularFeatureAvailable(GetModularFeatureName());
    }

    static IMegalithGraphFormatter& Get()
    {
        return IModularFeatures::Get().GetModularFeature<IMegalithGraphFormatter>(
            GetModularFeatureName());
    }
};
```

### Bridge Module Registration

```cpp
// MegalithBABridgeModule.cpp — depends on BlueprintAssist, NOT in MegalithCore
void FMegalithBABridgeModule::StartupModule()
{
    if (!GetDefault<UMegalithSettings>()->bEnableBlueprintAssist) return;

#if WITH_BLUEPRINT_ASSIST
    Formatter = MakeUnique<FMegalithBAFormatterImpl>();
    IModularFeatures::Get().RegisterModularFeature(
        IMegalithGraphFormatter::GetModularFeatureName(),
        Formatter.Get());
#endif
}

void FMegalithBABridgeModule::ShutdownModule()
{
#if WITH_BLUEPRINT_ASSIST
    if (Formatter.IsValid())
    {
        IModularFeatures::Get().UnregisterModularFeature(
            IMegalithGraphFormatter::GetModularFeatureName(),
            Formatter.Get());
        Formatter.Reset();
    }
#endif
}
```

The bridge module also uses `#if WITH_BLUEPRINT_ASSIST` (set by its `Build.cs` via
`Directory.Exists()`) so it compiles even when BA is absent — it just registers nothing.

### Consumer Pattern

```cpp
// MegalithBlueprintLayoutActions.cpp — zero BA dependency
bool bBAAvailable = IMegalithGraphFormatter::IsAvailable()
    && IMegalithGraphFormatter::Get().SupportsGraph(Graph);

if (bBAAvailable)
{
    int32 NodesFormatted = 0;
    FString ErrorMessage;
    if (IMegalithGraphFormatter::Get().FormatGraph(Graph, NodesFormatted, ErrorMessage))
    {
        // success path
    }
}
```

Always call `IsAvailable()` before `Get()`. `Get()` asserts if nothing is registered.

---

## How to Add a New Bridge Interface

Use this checklist when a core module needs to call into a new optional third-party API.

### 1. Define the interface in MegalithCore

Create `Source/MegalithCore/Public/IMegalithYourFeature.h`:

```cpp
#pragma once
#include "CoreMinimal.h"
#include "Features/IModularFeature.h"
#include "Features/IModularFeatures.h"

class IMegalithYourFeature : public IModularFeature
{
public:
    static FName GetModularFeatureName()
    {
        static const FName Name(TEXT("MegalithYourFeature"));
        return Name;
    }

    // Your virtual methods here
    virtual void DoThing() = 0;

    static bool IsAvailable()
    {
        return IModularFeatures::Get().IsModularFeatureAvailable(GetModularFeatureName());
    }

    static IMegalithYourFeature& Get()
    {
        return IModularFeatures::Get().GetModularFeature<IMegalithYourFeature>(
            GetModularFeatureName());
    }
};
```

No third-party headers. Only engine types. This header is safe to include anywhere.

### 2. Create the bridge module

Add `Source/MegalithYourBridge/` with:

- `MegalithYourBridge.Build.cs` — depends on `MegalithCore` + optional third party
  (use `Directory.Exists()` pattern from `OPTIONAL_MODULE_ARCHITECTURE.md` section 3.1)
- `MegalithYourBridgeModule.cpp` — registers/unregisters the implementation
- `MegalithYourFeatureImpl.h/.cpp` — concrete implementation behind `#if WITH_YOUR_PLUGIN`

Add the module to `Megalith.uplugin`:
```json
{
    "Name": "MegalithYourBridge",
    "Type": "Editor",
    "LoadingPhase": "Default"
}
```

> Use `Default` loading phase, not `PostEngineInit`. MegalithCore starts its HTTP server at
> `PostEngineInit`. Bridge modules must be registered before that. See
> `OPTIONAL_MODULE_ARCHITECTURE.md` section 12.2.

### 3. Implement the interface

```cpp
// MegalithYourFeatureImpl.h
#if WITH_YOUR_PLUGIN
#include "SomeThirdPartyHeader.h"

class FMegalithYourFeatureImpl : public IMegalithYourFeature
{
public:
    virtual void DoThing() override { /* call third party API */ }
};
#endif
```

### 4. Call from the consumer

```cpp
#include "IMegalithYourFeature.h"

if (IMegalithYourFeature::IsAvailable())
{
    IMegalithYourFeature::Get().DoThing();
}
```

The consumer (`MegalithBlueprint`, `MegalithMesh`, etc.) includes only
`IMegalithYourFeature.h` — no bridge or third-party headers.

### 5. Add a settings toggle

Add `bEnableYourPlugin` to `UMegalithSettings` in `MegalithSettings.h`. Check it in
`StartupModule()` before registering (see bridge module pattern above). This lets users
disable the integration without uninstalling the plugin.

---

## Module Graph

```
MegalithCore
  └── IMegalithGraphFormatter.h   (interface, no deps)

MegalithBABridge
  ├── depends on: MegalithCore, BlueprintAssist (optional via Build.cs)
  └── registers: FMegalithBAFormatterImpl → IMegalithGraphFormatter feature slot

MegalithBlueprint
  ├── depends on: MegalithCore only
  └── calls: IMegalithGraphFormatter::IsAvailable() / Get()
```

The bridge module is the only node that knows about both sides. Core modules stay clean.

---

## Key Constraints

- **Never** include third-party headers in the interface file (`IMegalithYourFeature.h`)
- **Always** check `IsAvailable()` before `Get()` — `Get()` will assert on failure
- **Always** unregister in `ShutdownModule()` — leaked registrations cause stale pointers
- **Use `TUniquePtr`** for the implementation instance in the bridge module (see bridge pattern above)
- The feature name string (`TEXT("MegalithGraphFormatter")`) must match exactly between
  `GetModularFeatureName()`, `RegisterModularFeature()`, and `UnregisterModularFeature()`
