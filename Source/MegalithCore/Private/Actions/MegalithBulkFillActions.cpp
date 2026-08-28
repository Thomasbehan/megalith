// SPDX-License-Identifier: MIT
// Central dispatcher actions for the bulk_fill + describe namespaces.
// Routes incoming JSON-RPC params to the per-namespace adapter table.
// Phase 0 — registered from FMegalithCoreModule::StartupModule.

#include "Actions/MegalithBulkFillActions.h"
#include "MegalithBulkFillRegistry.h"
#include "MegalithBulkFillTypes.h"
#include "Reflection/MegalithDryRunGuard.h"
#include "MegalithToolRegistry.h"
#include "MegalithJsonUtils.h"
#include "MegalithParamSchema.h"
#include "Dom/JsonObject.h"

namespace MegalithBulkFillActionsInternal
{
	static TSharedPtr<FJsonObject> BuildBulkFillSchema()
	{
		return FParamSchemaBuilder()
			.Required(TEXT("target_namespace"), TEXT("string"),
				TEXT("Adapter namespace ('blueprint', 'gas', 'inventory', 'ui', 'ai', 'niagara', 'material', 'audio', 'mesh', 'animation', 'logicdriver', 'combograph'). Alias: `namespace`."),
				{ TEXT("namespace") })
			.Required(TEXT("target"), TEXT("string"),
				TEXT("Asset path or adapter-defined target identifier (e.g. '/Game/Items/DA_HealingPotion')."))
			.Required(TEXT("tree"), TEXT("object"),
				TEXT("Nested JSON object of properties to walk against the target's reflection schema."))
			.Optional(TEXT("dry_run"), TEXT("boolean"),
				TEXT("If true, validate only — emit the would-be writes but do not persist."),
				TEXT("false"))
			.Optional(TEXT("strict"), TEXT("boolean"),
				TEXT("If true, promote silent drops / clamps / unknown-fields to hard errors."),
				TEXT("false"))
			.Build();
	}

	static TSharedPtr<FJsonObject> BuildBulkFillListNamespacesSchema()
	{
		return FParamSchemaBuilder().Build();
	}

	static TSharedPtr<FJsonObject> BuildDescribeSchema()
	{
		return FParamSchemaBuilder()
			.Required(TEXT("target_namespace"), TEXT("string"),
				TEXT("Adapter namespace whose schema should be introspected. Alias: `namespace`."),
				{ TEXT("namespace") })
			.Required(TEXT("target"), TEXT("string"),
				TEXT("Asset path or action name to describe. Alias: `action`."),
				{ TEXT("action") })
			.Build();
	}

	static TSharedPtr<FJsonObject> BuildDescribeListTargetsSchema()
	{
		return FParamSchemaBuilder()
			.Required(TEXT("target_namespace"), TEXT("string"),
				TEXT("Adapter namespace whose introspection inventory should be listed. Alias: `namespace`."),
				{ TEXT("namespace") })
			.Build();
	}

	// Serialise an FSchemaDescriptor tree to JSON for the response payload.
	static TSharedPtr<FJsonObject> DescriptorToJson(const FSchemaDescriptor& Desc)
	{
		TSharedPtr<FJsonObject> O = MakeShared<FJsonObject>();
		O->SetStringField(TEXT("field_path"), Desc.FieldPath);
		O->SetStringField(TEXT("type_name"), Desc.TypeName);
		O->SetStringField(TEXT("import_text_form"), Desc.ImportTextForm);
		O->SetBoolField(TEXT("required"), Desc.bRequired);
		O->SetBoolField(TEXT("set_once"), Desc.bSetOnce);
		O->SetBoolField(TEXT("pie_blocked"), Desc.bPieBlocked);
		if (!Desc.ConditionalOn.IsEmpty())
		{
			O->SetStringField(TEXT("conditional_on"), Desc.ConditionalOn);
		}
		O->SetNumberField(TEXT("range_min"), Desc.RangeMin);
		O->SetNumberField(TEXT("range_max"), Desc.RangeMax);
		if (Desc.EnumValues.Num() > 0)
		{
			TArray<TSharedPtr<FJsonValue>> Vals;
			for (const FString& E : Desc.EnumValues) { Vals.Add(MakeShared<FJsonValueString>(E)); }
			O->SetArrayField(TEXT("enum_values"), Vals);
		}
		if (Desc.Children.Num() > 0)
		{
			TArray<TSharedPtr<FJsonValue>> Kids;
			for (const FSchemaDescriptor& C : Desc.Children)
			{
				Kids.Add(MakeShared<FJsonValueObject>(DescriptorToJson(C)));
			}
			O->SetArrayField(TEXT("children"), Kids);
		}
		return O;
	}

	// --- bulk_fill.apply ---
	static FMegalithActionResult HandleBulkFillApply(const TSharedPtr<FJsonObject>& Params)
	{
		if (!Params.IsValid())
		{
			return FMegalithActionResult::Error(TEXT("bulk_fill.apply requires params"));
		}

		FBulkFillSpec Spec;
		Params->TryGetStringField(TEXT("target_namespace"), Spec.TargetNamespace);
		Params->TryGetStringField(TEXT("target"), Spec.TargetAsset);
		Spec.Tree = Params->GetObjectField(TEXT("tree"));
		Params->TryGetBoolField(TEXT("dry_run"), Spec.bDryRun);
		Params->TryGetBoolField(TEXT("strict"), Spec.bStrict);

		if (Spec.TargetNamespace.IsEmpty() || Spec.TargetAsset.IsEmpty() || !Spec.Tree.IsValid())
		{
			return FMegalithActionResult::Error(
				TEXT("bulk_fill.apply requires target_namespace, target, and tree (JSON object)"),
				FMegalithJsonUtils::ErrInvalidParams);
		}

		if (!FMegalithBulkFillRegistry::Get().HasAdapter(Spec.TargetNamespace))
		{
			return FMegalithActionResult::Error(
				FString::Printf(TEXT("no bulk_fill adapter registered for namespace '%s' (Phase 0 ships dispatcher only; per-namespace adapters land in Phases 1-5)"),
					*Spec.TargetNamespace),
				FMegalithJsonUtils::ErrOptionalDepUnavailable);
		}

		const FDryRunReport Report = FMegalithBulkFillRegistry::Get().DispatchBulkFill(Spec);
		return FMegalithActionResult::Success(FMegalithDryRunGuard::ReportToJson(Report));
	}

	// --- bulk_fill.list_namespaces ---
	static FMegalithActionResult HandleBulkFillListNamespaces(const TSharedPtr<FJsonObject>& /*Params*/)
	{
		TSharedPtr<FJsonObject> Out = MakeShared<FJsonObject>();
		const TArray<FString> Namespaces = FMegalithBulkFillRegistry::Get().GetRegisteredNamespaces();

		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FString& Ns : Namespaces)
		{
			TSharedPtr<FJsonObject> Entry = MakeShared<FJsonObject>();
			Entry->SetStringField(TEXT("namespace"), Ns);
			Entry->SetBoolField(TEXT("available"), true);
			Arr.Add(MakeShared<FJsonValueObject>(Entry));
		}
		Out->SetArrayField(TEXT("namespaces"), Arr);
		Out->SetNumberField(TEXT("count"), Namespaces.Num());
		return FMegalithActionResult::Success(Out);
	}

	// --- describe.schema ---
	static FMegalithActionResult HandleDescribeSchema(const TSharedPtr<FJsonObject>& Params)
	{
		if (!Params.IsValid())
		{
			return FMegalithActionResult::Error(TEXT("describe.schema requires params"));
		}

		FString TargetNamespace;
		FString Target;
		Params->TryGetStringField(TEXT("target_namespace"), TargetNamespace);
		Params->TryGetStringField(TEXT("target"), Target);

		if (TargetNamespace.IsEmpty() || Target.IsEmpty())
		{
			return FMegalithActionResult::Error(
				TEXT("describe.schema requires target_namespace and target"),
				FMegalithJsonUtils::ErrInvalidParams);
		}

		if (!FMegalithBulkFillRegistry::Get().HasAdapter(TargetNamespace))
		{
			return FMegalithActionResult::Error(
				FString::Printf(TEXT("no describe adapter registered for namespace '%s'"), *TargetNamespace),
				FMegalithJsonUtils::ErrOptionalDepUnavailable);
		}

		const FSchemaDescriptor Root = FMegalithBulkFillRegistry::Get().DispatchDescribe(TargetNamespace, Target);
		return FMegalithActionResult::Success(DescriptorToJson(Root));
	}

	// --- describe.list_targets ---
	static FMegalithActionResult HandleDescribeListTargets(const TSharedPtr<FJsonObject>& Params)
	{
		if (!Params.IsValid())
		{
			return FMegalithActionResult::Error(TEXT("describe.list_targets requires params"));
		}

		FString TargetNamespace;
		Params->TryGetStringField(TEXT("target_namespace"), TargetNamespace);

		if (TargetNamespace.IsEmpty())
		{
			return FMegalithActionResult::Error(
				TEXT("describe.list_targets requires target_namespace"),
				FMegalithJsonUtils::ErrInvalidParams);
		}

		const TArray<FString> Targets = FMegalithBulkFillRegistry::Get().DispatchListTargets(TargetNamespace);

		TSharedPtr<FJsonObject> Out = MakeShared<FJsonObject>();
		TArray<TSharedPtr<FJsonValue>> Arr;
		for (const FString& T : Targets) { Arr.Add(MakeShared<FJsonValueString>(T)); }
		Out->SetArrayField(TEXT("targets"), Arr);
		Out->SetStringField(TEXT("namespace"), TargetNamespace);
		Out->SetNumberField(TEXT("count"), Targets.Num());
		return FMegalithActionResult::Success(Out);
	}

	// --- describe::action_schema (gap #5) ---
	// Surfaces a registered ACTION's param schema (names / types / required / defaults /
	// aliases / descriptions) so callers stop trial-and-erroring param names. The data already
	// lives in FMegalithActionInfo.ParamSchema (the same object discover serializes as "params");
	// this just reads it back by (namespace, action). Closes the cause behind gaps #4/#12/#13.
	static TSharedPtr<FJsonObject> BuildDescribeActionSchemaSchema()
	{
		// RI ergonomics handover #6 (2026-05-29): canonical param is now
		// `target_action` for naming symmetry with `target_namespace`. The
		// historical name `action` is kept as a K2 alias so callers that
		// passed `{target_namespace, action}` still work — the registry's
		// ApplyAliases pass rewrites `action` → `target_action` before
		// the required-param check fires.
		return FParamSchemaBuilder()
			.Required(TEXT("target_namespace"), TEXT("string"),
				TEXT("Namespace that owns the action (e.g. \"blueprint\", \"ui\"). Alias: `namespace`."),
				{ TEXT("namespace") })
			.Required(TEXT("target_action"), TEXT("string"),
				TEXT("Action name whose param schema to return (e.g. \"add_nodes_bulk\"). Alias: `action`."),
				{ TEXT("action") })
			.Build();
	}

	static FMegalithActionResult HandleDescribeActionSchema(const TSharedPtr<FJsonObject>& Params)
	{
		// The registry's required-param validation already lists ALL missing
		// required params at once (MegalithToolRegistry.cpp ~line 319-349), so
		// callers see both `target_namespace` and `target_action` reported
		// together rather than one round-trip at a time. The handler itself
		// only re-reads the (now guaranteed) params from EffectiveParams.
		// `action` (the legacy name) has already been rewritten to
		// `target_action` by ApplyAliases — but we still fall back to
		// reading it directly as belt-and-braces back-compat in case any
		// future code path bypasses the alias rewrite.
		if (!Params.IsValid())
		{
			return FMegalithActionResult::Error(TEXT("describe.action_schema requires params"));
		}
		FString TargetNamespace;
		Params->TryGetStringField(TEXT("target_namespace"), TargetNamespace);
		FString ActionName;
		if (!Params->TryGetStringField(TEXT("target_action"), ActionName) || ActionName.IsEmpty())
		{
			Params->TryGetStringField(TEXT("action"), ActionName);
		}
		if (TargetNamespace.IsEmpty() || ActionName.IsEmpty())
		{
			// Defensive: registry's required-param check should have caught this
			// already. Kept as a safety net in case the schema is bypassed.
			TArray<FString> Missing;
			if (TargetNamespace.IsEmpty()) Missing.Add(TEXT("target_namespace"));
			if (ActionName.IsEmpty())      Missing.Add(TEXT("target_action"));
			return FMegalithActionResult::Error(FString::Printf(
				TEXT("missing required parameter(s): [%s]"),
				*FString::Join(Missing, TEXT(", "))),
				FMegalithJsonUtils::ErrInvalidParams);
		}

		FMegalithToolRegistry& Reg = FMegalithToolRegistry::Get();
		const TArray<FMegalithActionInfo> Actions = Reg.GetActions(TargetNamespace);
		const FMegalithActionInfo* Found = Actions.FindByPredicate(
			[&ActionName](const FMegalithActionInfo& Info){ return Info.Action == ActionName; });

		if (!Found)
		{
			return FMegalithActionResult::Error(FString::Printf(
				TEXT("action '%s' not found in namespace '%s'. Use megalith_discover(\"%s\") to list available actions."),
				*ActionName, *TargetNamespace, *TargetNamespace));
		}

		TSharedRef<FJsonObject> Out = MakeShared<FJsonObject>();
		Out->SetStringField(TEXT("namespace"), Found->Namespace);
		Out->SetStringField(TEXT("action"), Found->Action);
		Out->SetStringField(TEXT("description"), Found->Description);
		if (!Found->Category.IsEmpty())
		{
			Out->SetStringField(TEXT("category"), Found->Category);
		}
		// Same JSON-Schema "properties" object discover serializes as "params".
		Out->SetObjectField(TEXT("params"),
			Found->ParamSchema.IsValid() ? Found->ParamSchema : MakeShared<FJsonObject>());
		return FMegalithActionResult::Success(Out);
	}
} // namespace MegalithBulkFillActionsInternal

void FMegalithBulkFillActions::RegisterAll()
{
	using namespace MegalithBulkFillActionsInternal;

	FMegalithToolRegistry& Reg = FMegalithToolRegistry::Get();

	// Reminder per ref_megalith_action_registry_api.md:
	//   RegisterAction(namespace, action, DESCRIPTION, handler, ParamSchema, Category)
	// Description is the THIRD param (NOT a keyword arg; positional).

	Reg.RegisterAction(
		TEXT("bulk_fill"),
		TEXT("apply"),
		TEXT("Apply a JSON-tree fill to an asset via the target namespace's adapter. Supports dry_run + strict."),
		FMegalithActionHandler::CreateStatic(&HandleBulkFillApply),
		BuildBulkFillSchema());

	Reg.RegisterAction(
		TEXT("bulk_fill"),
		TEXT("list_namespaces"),
		TEXT("List target_namespace values the bulk_fill registry currently knows about (one row per registered adapter)."),
		FMegalithActionHandler::CreateStatic(&HandleBulkFillListNamespaces),
		BuildBulkFillListNamespacesSchema());

	Reg.RegisterAction(
		TEXT("describe"),
		TEXT("schema"),
		TEXT("Return a rich FSchemaDescriptor tree (type names, ImportText forms, enum-value lists, clamp ranges, nested children) for an asset/action via its namespace adapter."),
		FMegalithActionHandler::CreateStatic(&HandleDescribeSchema),
		BuildDescribeSchema());

	Reg.RegisterAction(
		TEXT("describe"),
		TEXT("list_targets"),
		TEXT("List the asset paths / action names the describe adapter can introspect for a given target_namespace."),
		FMegalithActionHandler::CreateStatic(&HandleDescribeListTargets),
		BuildDescribeListTargetsSchema());

	Reg.RegisterAction(
		TEXT("describe"),
		TEXT("action_schema"),
		TEXT("Return a registered ACTION's param schema (names, types, required, defaults, aliases, descriptions) by (target_namespace, action). Closes param-name discoverability so callers stop trial-and-erroring param names."),
		FMegalithActionHandler::CreateStatic(&HandleDescribeActionSchema),
		BuildDescribeActionSchemaSchema());

	UE_LOG(LogMegalith, Log, TEXT("MegalithBulkFillActions: registered 2 namespaces (bulk_fill + describe) with 5 actions total"));
}

void FMegalithBulkFillActions::UnregisterAll()
{
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("bulk_fill"));
	FMegalithToolRegistry::Get().UnregisterNamespace(TEXT("describe"));
}
