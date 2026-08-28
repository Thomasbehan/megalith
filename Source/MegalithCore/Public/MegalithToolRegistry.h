#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

/** Result of an action execution */
struct FMegalithActionResult
{
	bool bSuccess = false;
	TSharedPtr<FJsonObject> Result;
	FString ErrorMessage;
	int32 ErrorCode = 0;

	// Survivor C (plan §3.C) — optional structured payload attached to error
	// responses. Currently used to carry `error.data.suggestions` (top-3
	// did-you-mean candidates) on Unknown-action / Unknown-namespace errors;
	// available for any future structured-error use. Null on Success and on
	// errors that don't carry structured data.
	TSharedPtr<FJsonValue> ErrorData;

	static FMegalithActionResult Success(const TSharedPtr<FJsonObject>& InResult)
	{
		FMegalithActionResult R;
		R.bSuccess = true;
		R.Result = InResult;
		return R;
	}

	static FMegalithActionResult Error(const FString& Message, int32 Code = -32603)
	{
		FMegalithActionResult R;
		R.bSuccess = false;
		R.ErrorMessage = Message;
		R.ErrorCode = Code;
		return R;
	}

	/** Survivor C — attach a JSON-object data payload to an existing error. */
	FMegalithActionResult& WithErrorData(const TSharedPtr<FJsonObject>& Data)
	{
		if (Data.IsValid()) { ErrorData = MakeShared<FJsonValueObject>(Data); }
		else { ErrorData.Reset(); }
		return *this;
	}
};

/** Delegate type for action handlers */
DECLARE_DELEGATE_RetVal_OneParam(FMegalithActionResult, FMegalithActionHandler, const TSharedPtr<FJsonObject>& /* Params */);

/** Metadata describing a registered action */
struct FMegalithActionInfo
{
	FString Namespace;
	FString Action;
	FString Description;
	FString Category;                     // Optional sub-grouping within a namespace (e.g. "CommonUI" inside "ui"). Empty = uncategorized.
	TSharedPtr<FJsonObject> ParamSchema;  // JSON Schema for parameter validation

	// Survivor A (plan §3.A) — MCP-spec tool annotation hints. Only emitted on
	// `tools/list` when at least one is non-default; per-call runtime cost is zero.
	// For individually-registered tools (`megalith_*`) the hints are read from the
	// action's own info. For namespace dispatcher tools (`*_query`) the hints come
	// from FMegalithToolRegistry::GetDispatcherAnnotations() — see that path for
	// the rationale (sibling actions in the same dispatcher can disagree on
	// destructive/read-only, so the dispatcher-level annotation is authoritative).
	bool bReadOnlyHint   = false;
	bool bDestructiveHint = false;
	bool bIdempotentHint = false;
	FString Title;
};

/**
 * Survivor A (plan §3.A) — per-namespace dispatcher annotations.
 * Used for namespace dispatcher tools (`source_query` etc.) where the four MCP
 * hint fields apply to the WHOLE dispatcher rather than any single action. Held
 * separately from FMegalithActionInfo because the dispatcher is not a
 * registered action — it is synthesised inside HandleToolsList at serialize time.
 */
struct FMegalithDispatcherAnnotations
{
	bool bReadOnlyHint   = false;
	bool bDestructiveHint = false;
	bool bIdempotentHint = false;
	FString Title;

	/** Helper: true iff any hint is non-default. Drives "do we emit annotations on the wire?" */
	bool IsAnyNonDefault() const
	{
		return bReadOnlyHint || bDestructiveHint || bIdempotentHint || !Title.IsEmpty();
	}
};

/**
 * Central registry for all Megalith tool actions.
 * Domain modules register actions here. The HTTP server dispatches through this.
 */
class MEGALITHCORE_API FMegalithToolRegistry
{
public:
	static FMegalithToolRegistry& Get();

	/**
	 * Register an action handler.
	 * @param Namespace   The tool namespace (e.g., "blueprint", "material")
	 * @param Action      The action name (e.g., "list_graphs", "get_node")
	 * @param Description Human-readable description of what this action does
	 * @param Handler     The delegate to execute
	 * @param ParamSchema Optional JSON Schema describing expected parameters
	 */
	void RegisterAction(
		const FString& Namespace,
		const FString& Action,
		const FString& Description,
		const FMegalithActionHandler& Handler,
		const TSharedPtr<FJsonObject>& ParamSchema = nullptr,
		const FString& Category = FString()  // Optional sub-group within namespace — defaults to uncategorized
	);

	/** Unregister all actions in a namespace (called during module shutdown) */
	void UnregisterNamespace(const FString& Namespace);

	/** Execute an action by namespace + action name */
	FMegalithActionResult ExecuteAction(const FString& Namespace, const FString& Action, const TSharedPtr<FJsonObject>& Params);

	/** Get all registered namespaces */
	TArray<FString> GetNamespaces() const;

	/** Get all actions in a namespace */
	TArray<FMegalithActionInfo> GetActions(const FString& Namespace) const;

	/** Get all actions across all namespaces */
	TArray<FMegalithActionInfo> GetAllActions() const;

	/** Check if a specific action exists */
	bool HasAction(const FString& Namespace, const FString& Action) const;

	/** Get total number of registered actions */
	int32 GetActionCount() const;

	/**
	 * Survivor A (plan §3.A) — Set MCP hint annotations for a namespace dispatcher
	 * tool (e.g. `source_query`). These are serialized into `tools/list` under the
	 * dispatcher tool's `annotations` sub-object. Only namespaces whose dispatcher
	 * is audited as safe (read-only / idempotent) should call this. Defaults are
	 * preserved when no call is made — so untagged dispatchers stay defaulted.
	 *
	 * Thread-safe: takes RegistryLock internally.
	 */
	void SetDispatcherAnnotations(const FString& Namespace, const FMegalithDispatcherAnnotations& Annotations);

	/**
	 * Survivor A — Look up dispatcher annotations for a namespace. Returns a
	 * default-constructed (all-false / empty-title) struct if the namespace was
	 * never annotated. Used by HandleToolsList to decide whether to emit the
	 * MCP `annotations` block.
	 */
	FMegalithDispatcherAnnotations GetDispatcherAnnotations(const FString& Namespace) const;

	/**
	 * Survivor A (plan §3.A) — Set MCP hint annotations on an already-registered
	 * action. Used for individually-registered top-level tools (`megalith_discover`,
	 * `megalith_status`, etc.). No-op if the action is not registered (safe to
	 * call defensively at module init order boundaries).
	 *
	 * Thread-safe: takes RegistryLock internally.
	 */
	void SetActionAnnotations(
		const FString& Namespace,
		const FString& Action,
		bool bReadOnly,
		bool bDestructive,
		bool bIdempotent,
		const FString& Title);

private:
	FMegalithToolRegistry() = default;

	struct FRegisteredAction
	{
		FMegalithActionInfo Info;
		FMegalithActionHandler Handler;
	};

	/** Map of "namespace.action" → registered action */
	TMap<FString, FRegisteredAction> Actions;

	/** Map of namespace → list of action keys */
	TMap<FString, TArray<FString>> NamespaceActions;

	/** Survivor A — Map of namespace → dispatcher-level MCP hint annotations. */
	TMap<FString, FMegalithDispatcherAnnotations> DispatcherAnnotations;

	static FString MakeKey(const FString& Namespace, const FString& Action)
	{
		return Namespace + TEXT(".") + Action;
	}

	mutable FCriticalSection RegistryLock;
};
