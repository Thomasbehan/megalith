# Contributing to Megalith

Thanks for your interest in contributing. This guide covers everything you need to get started.

## Dev Environment Setup

### Prerequisites

- **Unreal Engine 5.7+** (source or launcher build)
- **Windows, macOS, or Linux** — see [README Installation](README.md#installation) for per-platform proxy setup
- **Python 3.10+** (only needed for engine source indexing and for the cross-platform MCP proxy on macOS/Linux)
- **Git**

### Clone & Build

```bash
# Clone into your project's Plugins directory
cd YourProject/Plugins
git clone https://github.com/tumourlove/megalith.git Megalith

# Or clone the standalone development repo
git clone https://github.com/tumourlove/megalith.git C:\Projects\Megalith
```

Generate project files and build from your UE project as usual. Megalith is an editor-only plugin — all 13 modules have `Type: "Editor"`.

### Development Workflow

Clone the repo into your UE project's `Plugins/` folder and develop in-place:

```
YourProject/Plugins/Megalith/   — edit, build, commit, push from here
```

---

## Code Structure

Megalith has 13 modules, each owning a specific domain:

| Module | Namespace | Actions | What It Does |
|--------|-----------|---------|--------------|
| **MegalithCore** | `megalith` | 4 | HTTP server, tool registry, discovery, settings, auto-updater |
| **MegalithBlueprint** | `blueprint` | 86 | Blueprint read/write, variable/component/graph CRUD, node operations, compile, auto-layout |
| **MegalithMaterial** | `material` | 57 | Material graph editing, inspection, CRUD, instances, functions, HLSL |
| **MegalithAnimation** | `animation` | 115 | Sequences, montages, ABPs, curves, notifies, skeletons, PoseSearch, IKRig, Control Rig |
| **MegalithNiagara** | `niagara` | 96 | Particle systems, emitters, modules, renderers, HLSL, dynamic inputs, event handlers, sim stages |
| **MegalithMesh** | `mesh` | 242 | Mesh inspection, scene manipulation, spatial queries, blockout, procedural geometry, lighting, audio, town gen (197 core + 45 experimental) |
| **MegalithEditor** | `editor` | 19 | Build triggers, live compile, log capture, crash context, scene capture, texture import |
| **MegalithConfig** | `config` | 6 | INI resolution, explain, diff, search |
| **MegalithIndex** | `project` | 7 | SQLite FTS5 deep project indexer |
| **MegalithSource** | `source` | 11 | Engine source lookup, call graphs, class hierarchy |
| **MegalithUI** | `ui` | 42 | Widget Blueprint CRUD, templates, styling, animation, settings scaffolding, accessibility |
| **MegalithGAS** | `gas` | 130 | Gameplay Ability System: abilities, attributes, effects, ASC, tags, cues, targeting, input, inspect, scaffold |
| **MegalithBABridge** | — | 0 | Optional Blueprint Assist integration bridge (no MCP actions — integration only) |

Each module follows the same file structure:

```
Source/MegalithFoo/
  Public/
    MegalithFooModule.h
    MegalithFooActions.h
  Private/
    MegalithFooModule.cpp
    MegalithFooActions.cpp
```

---

## How to Add a New Action

Actions are the atomic units of functionality. Each domain module registers actions with the central `FMegalithToolRegistry`.

### 1. Declare the handler

In your module's `Actions.h`, add a static method:

```cpp
static TSharedPtr<FJsonObject> HandleMyAction(const TSharedPtr<FJsonObject>& Params);
```

### 2. Implement the handler

In your module's `Actions.cpp`:

```cpp
TSharedPtr<FJsonObject> FMegalithFooActions::HandleMyAction(const TSharedPtr<FJsonObject>& Params)
{
    // Extract params
    FString AssetPath = Params->GetStringField(TEXT("asset_path"));

    // Do work (on game thread — handlers run on game thread via AsyncTask)

    // Return result
    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("status"), TEXT("success"));
    return Result;
}
```

### 3. Register in StartupModule

In your module's `Module.cpp`:

```cpp
void FMegalithFooModule::StartupModule()
{
    FMegalithToolRegistry& Registry = FMegalithToolRegistry::Get();

    Registry.RegisterAction(
        TEXT("foo"),                    // namespace
        TEXT("my_action"),             // action name
        TEXT("Description of what it does"),
        TEXT("{\"asset_path\": \"string (required)\"}"),  // param schema
        &FMegalithFooActions::HandleMyAction
    );
}
```

### 4. Update the skill

If your domain has a skill in `Skills/`, add the new action to its action table.

---

## How to Add a New Indexer

MegalithIndex uses a plugin-style indexer system. Each indexer implements `IMegalithIndexer`.

### 1. Create the indexer class

```cpp
class FMyIndexer : public IMegalithIndexer
{
public:
    virtual TArray<UClass*> GetSupportedClasses() const override
    {
        return { UMyAssetClass::StaticClass() };
    }

    virtual void IndexAsset(
        FMegalithIndexDatabase& DB,
        const FAssetData& AssetData,
        UObject* LoadedAsset) override
    {
        // Extract data and write to DB using prepared statements
        DB.InsertNode(AssetId, NodeName, NodeClass, NodeType);
    }

    virtual FString GetName() const override { return TEXT("MyIndexer"); }
};
```

### 2. Register in the subsystem

Add your indexer to `UMegalithIndexSubsystem::Initialize()`:

```cpp
Indexers.Add(MakeUnique<FMyIndexer>());
```

### 3. Add DB tables if needed

If your indexer needs new tables, add the schema in `FMegalithIndexDatabase::CreateSchema()`. Follow the existing pattern with `CREATE TABLE IF NOT EXISTS`.

---

## Coding Conventions

### General

- **UE coding standard** — `F` prefix for structs, `U` for UObjects, `T` for templates, `b` prefix for bools
- **Static action handlers** — All action classes use static methods, no instance state
- **Game thread execution** — Handlers execute on the game thread via `AsyncTask(ENamedThreads::GameThread, ...)`

### Logging

Use the `LogMegalith` category for all log output:

```cpp
UE_LOG(LogMegalith, Log, TEXT("Something happened: %s"), *Value);
UE_LOG(LogMegalith, Warning, TEXT("Something unexpected: %s"), *Value);
UE_LOG(LogMegalith, Error, TEXT("Something failed: %s"), *Error);
```

Do **not** use `LogTemp`.

### Database Access

All SQL must use prepared statements to prevent injection:

```cpp
FSQLitePreparedStatement Stmt;
Stmt.Create(*Database, TEXT("INSERT INTO nodes (asset_id, name) VALUES (?, ?)"));
Stmt.SetBindingValueByIndex(1, AssetId);
Stmt.SetBindingValueByIndex(2, NodeName);
Stmt.Execute();
```

Never use string formatting to build SQL queries.

### Error Handling

Return errors as JSON with a clear message:

```cpp
TSharedPtr<FJsonObject> Error = MakeShared<FJsonObject>();
Error->SetStringField(TEXT("error"), TEXT("Asset not found"));
Error->SetStringField(TEXT("asset_path"), AssetPath);
return Error;
```

### Asset Loading

Use the 4-tier fallback in `FMegalithAssetUtils`:

```cpp
UBlueprint* BP = FMegalithAssetUtils::LoadAssetByPath<UBlueprint>(AssetPath);
```

This handles: StaticLoadObject -> PackageName.ObjectName -> FindObject+_C suffix -> ForEachObjectWithPackage.

---

## Testing

Megalith exposes a Streamable HTTP MCP server. You can test with curl or any MCP client.

### curl Examples

**Discover available tools:**
```bash
curl -X POST http://localhost:9316/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":1,"method":"tools/list"}'
```

**Call an action:**
```bash
curl -X POST http://localhost:9316/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"blueprint_query","arguments":{"action":"list_graphs","asset_path":"/Game/MyBlueprint.MyBlueprint"}}}'
```

**Check server status:**
```bash
curl -X POST http://localhost:9316/mcp \
  -H "Content-Type: application/json" \
  -d '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"megalith_status","arguments":{}}}'
```

### MCP Client

Configure your `.mcp.json` (see `Templates/.mcp.json.example`):

```json
{
  "mcpServers": {
    "megalith": {
      "type": "streamableHttp",
      "url": "http://localhost:9316/mcp"
    }
  }
}
```

Then use Claude Code or any MCP-compatible client to interact with the tools.

### What to Verify

- Your action appears in `megalith_discover` output
- Valid params return correct results
- Missing/invalid params return clear error JSON (not crashes)
- Asset paths with various formats work (the 4-tier fallback)

---

## Pull Request Process & Contribution Guidelines

We follow a clear, contributor-friendly workflow where author credit and Git history are strictly respected.

### Workflow

1. **Branch from `master`**
   Create a dedicated branch for your change with a clear, descriptive prefix:
   ```bash
   git checkout -b feat/your-feature-name
   # or
   git checkout -b fix/issue-being-resolved
   ```

2. **Develop & Test Direct**
   - Build and test directly in your Unreal Engine project (`Plugins/Megalith`).
   - Execute tests directly via MCP tool calls or HTTP endpoints to ensure clear visibility into parameters, responses, and errors.
   - If adding new actions, verify the action registers with `megalith_discover` and schema validates with `describe_query`.

3. **Documentation Updates**
   If introducing or modifying actions:
   - Update the corresponding spec in `Docs/specs/SPEC_<Module>.md`.
   - Update any relevant skills in `Skills/`.
   - Update `README.md` action counts if adding new namespaces/actions.

4. **Open a Pull Request**
   - Push your branch and open a PR against `master`.
   - Describe what the PR accomplishes, why it is needed, and how it was verified.
   - Request review from maintainers.

5. **Review & Iterate**
   - Address any reviewer feedback or suggestions.
   - Push follow-up commits to your branch as needed during the review process.

6. **Approval & Merge**
   - Once approved, PRs are integrated via **Squash and Merge** (or standard merge commits when preserving detailed multi-step history is beneficial).
   - Author attribution, commit messages, and contributor recognition are preserved in the commit history.

### Commit Format
- Use conventional commit style for clear changelogs:
  - `feat:` for new actions, namespaces, or capabilities.
  - `fix:` for bug fixes, crash resolutions, or schema corrections.
  - `docs:` for documentation, guides, or skill updates.
  - `refactor:` for code restructuring without behavioral changes.
  - `test:` for test additions or verification harnesses.

---

## Architecture Notes

- **Discovery/dispatch pattern** — Each domain exposes one `{namespace}_query(action, params)` MCP tool. The registry dispatches to the correct handler. This keeps AI context lean (15 tools instead of 815 individual endpoints).
- **Thread safety** — `FMegalithToolRegistry` releases its lock before executing handlers. DB access uses `FCriticalSection`.
- **Stateless server** — No session tracking. Every request is independent.
- **MCP protocol version** — 2025-03-26, Streamable HTTP transport.

---

## License

By contributing, you agree that your contributions will be licensed under the [MIT License](LICENSE).
