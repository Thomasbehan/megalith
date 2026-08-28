// MegalithUIBindingActions.cpp
#include "MegalithUIBindingActions.h"
#include "MegalithUIInternal.h"
#include "MegalithParamSchema.h"
#include "Components/ListView.h"
#include "Components/TileView.h"
#include "UObject/UnrealType.h"
#include "Kismet2/KismetEditorUtilities.h"

void FMegalithUIBindingActions::RegisterActions(FMegalithToolRegistry& Registry)
{
    Registry.RegisterAction(
        TEXT("ui"), TEXT("list_widget_events"),
        TEXT("List all bindable events on widgets in a Widget Blueprint"),
        FMegalithActionHandler::CreateStatic(&HandleListWidgetEvents),
        FParamSchemaBuilder()
            .RequiredAssetPath(TEXT("asset_path"), TEXT("Widget Blueprint asset path"))
            .Optional(TEXT("widget_name"), TEXT("string"), TEXT("Specific widget name (omit for all widgets)"))
            .Build()
    );

    Registry.RegisterAction(
        TEXT("ui"), TEXT("list_widget_properties"),
        TEXT("List all settable properties on a widget"),
        FMegalithActionHandler::CreateStatic(&HandleListWidgetProperties),
        FParamSchemaBuilder()
            .RequiredAssetPath(TEXT("asset_path"), TEXT("Widget Blueprint asset path"))
            .Required(TEXT("widget_name"), TEXT("string"), TEXT("Target widget name"))
            .Build()
    );

    Registry.RegisterAction(
        TEXT("ui"), TEXT("setup_list_view"),
        TEXT("Configure a ListView or TileView with entry widget class and dimensions"),
        FMegalithActionHandler::CreateStatic(&HandleSetupListView),
        FParamSchemaBuilder()
            .RequiredAssetPath(TEXT("asset_path"), TEXT("Widget Blueprint asset path"))
            .Required(TEXT("list_widget_name"), TEXT("string"), TEXT("Name of the ListView or TileView widget"))
            .Required(TEXT("entry_widget_class"), TEXT("string"), TEXT("Asset path to the entry Widget Blueprint"))
            .Optional(TEXT("entry_height"), TEXT("number"), TEXT("Entry height in pixels (default: 50)"), TEXT("50"))
            .Optional(TEXT("entry_width"), TEXT("number"), TEXT("Entry width in pixels (for TileView, default: 100)"), TEXT("100"))
            .Build()
    );

    Registry.RegisterAction(
        TEXT("ui"), TEXT("get_widget_bindings"),
        TEXT("Get all property bindings (data binding) on a Widget Blueprint"),
        FMegalithActionHandler::CreateStatic(&HandleGetWidgetBindings),
        FParamSchemaBuilder()
            .RequiredAssetPath(TEXT("asset_path"), TEXT("Widget Blueprint asset path"))
            .Build()
    );
}

// --- list_widget_events ---
FMegalithActionResult FMegalithUIBindingActions::HandleListWidgetEvents(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath = Params->GetStringField(TEXT("asset_path"));
    FString WidgetNameFilter = Params->GetStringField(TEXT("widget_name"));

    FMegalithActionResult Err;
    UWidgetBlueprint* WBP = MegalithUIInternal::LoadWidgetBlueprint(AssetPath, Err);
    if (!WBP) return Err;

    if (!WBP->WidgetTree)
    {
        return FMegalithActionResult::Error(TEXT("WidgetTree is null"));
    }

    TArray<TSharedPtr<FJsonValue>> EventsArray;

    auto ProcessWidget = [&](UWidget* Widget)
    {
        if (!Widget) return;
        if (!WidgetNameFilter.IsEmpty() && Widget->GetName() != WidgetNameFilter) return;

        UClass* WidgetClass = Widget->GetClass();
        for (TFieldIterator<FMulticastDelegateProperty> PropIt(WidgetClass); PropIt; ++PropIt)
        {
            FMulticastDelegateProperty* DelegateProp = *PropIt;
            if (!DelegateProp) continue;

            // Only include events that are BlueprintAssignable
            if (!DelegateProp->HasAnyPropertyFlags(CPF_BlueprintAssignable)) continue;

            TSharedPtr<FJsonObject> EventObj = MakeShared<FJsonObject>();
            EventObj->SetStringField(TEXT("widget_name"), Widget->GetName());
            EventObj->SetStringField(TEXT("event_name"), DelegateProp->GetName());
            EventObj->SetStringField(TEXT("delegate_type"), DelegateProp->GetClass()->GetName());

            // Get the function signature if available
            if (const UFunction* SignatureFunc = DelegateProp->SignatureFunction)
            {
                TArray<TSharedPtr<FJsonValue>> ParamsArr;
                for (TFieldIterator<FProperty> ParamIt(SignatureFunc); ParamIt; ++ParamIt)
                {
                    if ((*ParamIt)->HasAnyPropertyFlags(CPF_Parm) &&
                        !(*ParamIt)->HasAnyPropertyFlags(CPF_ReturnParm))
                    {
                        TSharedPtr<FJsonObject> ParamObj = MakeShared<FJsonObject>();
                        ParamObj->SetStringField(TEXT("name"), (*ParamIt)->GetName());
                        ParamObj->SetStringField(TEXT("type"), (*ParamIt)->GetCPPType());
                        ParamsArr.Add(MakeShared<FJsonValueObject>(ParamObj));
                    }
                }
                if (ParamsArr.Num() > 0)
                {
                    EventObj->SetArrayField(TEXT("parameters"), ParamsArr);
                }
            }

            EventsArray.Add(MakeShared<FJsonValueObject>(EventObj));
        }
    };

    // Iterate all widgets in the tree
    WBP->WidgetTree->ForEachWidget([&](UWidget* Widget)
    {
        ProcessWidget(Widget);
    });

    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset_path"), AssetPath);
    Result->SetArrayField(TEXT("events"), EventsArray);
    Result->SetNumberField(TEXT("count"), EventsArray.Num());
    return FMegalithActionResult::Success(Result);
}

// --- list_widget_properties ---
FMegalithActionResult FMegalithUIBindingActions::HandleListWidgetProperties(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath = Params->GetStringField(TEXT("asset_path"));
    FString WidgetName = Params->GetStringField(TEXT("widget_name"));

    FMegalithActionResult Err;
    UWidgetBlueprint* WBP = MegalithUIInternal::LoadWidgetBlueprint(AssetPath, Err);
    if (!WBP) return Err;

    if (!WBP->WidgetTree)
    {
        return FMegalithActionResult::Error(TEXT("WidgetTree is null"));
    }

    UWidget* Widget = WBP->WidgetTree->FindWidget(FName(*WidgetName));
    if (!Widget)
    {
        return FMegalithActionResult::Error(
            FString::Printf(TEXT("Widget '%s' not found"), *WidgetName));
    }

    TArray<TSharedPtr<FJsonValue>> PropsArray;
    UClass* WidgetClass = Widget->GetClass();

    for (TFieldIterator<FProperty> PropIt(WidgetClass); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;
        if (!Prop) continue;

        // Only include properties that are editable or blueprint-accessible
        bool bEditable = Prop->HasAnyPropertyFlags(CPF_Edit);
        bool bBPReadWrite = Prop->HasAnyPropertyFlags(CPF_BlueprintVisible) &&
                            !Prop->HasAnyPropertyFlags(CPF_BlueprintReadOnly);

        if (!bEditable && !bBPReadWrite) continue;

        // Skip delegate properties (those are events, not settable values)
        if (Prop->IsA<FMulticastDelegateProperty>() || Prop->IsA<FDelegateProperty>()) continue;

        TSharedPtr<FJsonObject> PropObj = MakeShared<FJsonObject>();
        PropObj->SetStringField(TEXT("property_name"), Prop->GetName());
        PropObj->SetStringField(TEXT("type"), Prop->GetCPPType());

        // Category
        FString Category = Prop->GetMetaData(TEXT("Category"));
        if (!Category.IsEmpty())
        {
            PropObj->SetStringField(TEXT("category"), Category);
        }

        // Current value as string
        FString ValueStr;
        const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Widget);
        if (ValuePtr)
        {
            Prop->ExportTextItem_Direct(ValueStr, ValuePtr, nullptr, Widget, PPF_None);
            if (ValueStr.Len() <= 256) // Don't serialize huge values
            {
                PropObj->SetStringField(TEXT("current_value"), ValueStr);
            }
        }

        PropsArray.Add(MakeShared<FJsonValueObject>(PropObj));
    }

    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset_path"), AssetPath);
    Result->SetStringField(TEXT("widget_name"), WidgetName);
    Result->SetStringField(TEXT("widget_class"), WidgetClass->GetName());
    Result->SetArrayField(TEXT("properties"), PropsArray);
    Result->SetNumberField(TEXT("count"), PropsArray.Num());
    return FMegalithActionResult::Success(Result);
}

// --- setup_list_view ---
FMegalithActionResult FMegalithUIBindingActions::HandleSetupListView(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath = Params->GetStringField(TEXT("asset_path"));
    FString ListWidgetName = Params->GetStringField(TEXT("list_widget_name"));
    FString EntryWidgetClass = Params->GetStringField(TEXT("entry_widget_class"));

    double EntryHeight = 50.0;
    if (Params->HasField(TEXT("entry_height")))
    {
        EntryHeight = Params->GetNumberField(TEXT("entry_height"));
    }
    double EntryWidth = 100.0;
    if (Params->HasField(TEXT("entry_width")))
    {
        EntryWidth = Params->GetNumberField(TEXT("entry_width"));
    }

    FMegalithActionResult Err;
    UWidgetBlueprint* WBP = MegalithUIInternal::LoadWidgetBlueprint(AssetPath, Err);
    if (!WBP) return Err;

    if (!WBP->WidgetTree)
    {
        return FMegalithActionResult::Error(TEXT("WidgetTree is null"));
    }

    UWidget* Widget = WBP->WidgetTree->FindWidget(FName(*ListWidgetName));
    if (!Widget)
    {
        return FMegalithActionResult::Error(
            FString::Printf(TEXT("Widget '%s' not found"), *ListWidgetName));
    }

    // Load the entry widget blueprint to get its generated class
    UWidgetBlueprint* EntryWBP = nullptr;
    {
        FMegalithActionResult EntryErr;
        EntryWBP = MegalithUIInternal::LoadWidgetBlueprint(EntryWidgetClass, EntryErr);
        if (!EntryWBP)
        {
            return FMegalithActionResult::Error(
                FString::Printf(TEXT("Entry widget class not found: %s"), *EntryWidgetClass));
        }
    }

    UClass* EntryClass = EntryWBP->GeneratedClass;
    if (!EntryClass)
    {
        return FMegalithActionResult::Error(TEXT("Entry widget blueprint has no generated class — compile it first"));
    }

    bool bConfigured = false;
    FString WidgetType;

    if (UListView* ListView = Cast<UListView>(Widget))
    {
        // Set EntryWidgetClass via the property
        FProperty* EntryProp = ListView->GetClass()->FindPropertyByName(TEXT("EntryWidgetClass"));
        if (EntryProp)
        {
            FClassProperty* ClassProp = CastField<FClassProperty>(EntryProp);
            if (ClassProp)
            {
                ClassProp->SetPropertyValue_InContainer(ListView, EntryClass);
            }
        }

        // Set entry height
        FProperty* HeightProp = ListView->GetClass()->FindPropertyByName(TEXT("EntryHeight"));
        if (HeightProp)
        {
            FFloatProperty* FloatProp = CastField<FFloatProperty>(HeightProp);
            if (FloatProp)
            {
                FloatProp->SetPropertyValue_InContainer(ListView, static_cast<float>(EntryHeight));
            }
        }

        bConfigured = true;
        WidgetType = TEXT("ListView");
    }
    else if (UTileView* TileView = Cast<UTileView>(Widget))
    {
        FProperty* EntryProp = TileView->GetClass()->FindPropertyByName(TEXT("EntryWidgetClass"));
        if (EntryProp)
        {
            FClassProperty* ClassProp = CastField<FClassProperty>(EntryProp);
            if (ClassProp)
            {
                ClassProp->SetPropertyValue_InContainer(TileView, EntryClass);
            }
        }

        FProperty* HeightProp = TileView->GetClass()->FindPropertyByName(TEXT("EntryHeight"));
        if (HeightProp)
        {
            FFloatProperty* FloatProp = CastField<FFloatProperty>(HeightProp);
            if (FloatProp)
            {
                FloatProp->SetPropertyValue_InContainer(TileView, static_cast<float>(EntryHeight));
            }
        }

        FProperty* WidthProp = TileView->GetClass()->FindPropertyByName(TEXT("EntryWidth"));
        if (WidthProp)
        {
            FFloatProperty* FloatProp = CastField<FFloatProperty>(WidthProp);
            if (FloatProp)
            {
                FloatProp->SetPropertyValue_InContainer(TileView, static_cast<float>(EntryWidth));
            }
        }

        bConfigured = true;
        WidgetType = TEXT("TileView");
    }
    else
    {
        return FMegalithActionResult::Error(
            FString::Printf(TEXT("Widget '%s' is not a ListView or TileView (is %s)"),
                *ListWidgetName, *Widget->GetClass()->GetName()));
    }

    WBP->MarkPackageDirty();
    FKismetEditorUtilities::CompileBlueprint(WBP);

    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset_path"), AssetPath);
    Result->SetStringField(TEXT("list_widget_name"), ListWidgetName);
    Result->SetStringField(TEXT("widget_type"), WidgetType);
    Result->SetStringField(TEXT("entry_widget_class"), EntryWidgetClass);
    Result->SetNumberField(TEXT("entry_height"), EntryHeight);
    if (WidgetType == TEXT("TileView"))
    {
        Result->SetNumberField(TEXT("entry_width"), EntryWidth);
    }
    Result->SetBoolField(TEXT("configured"), bConfigured);
    Result->SetBoolField(TEXT("compiled"), true);
    return FMegalithActionResult::Success(Result);
}

// --- get_widget_bindings ---
FMegalithActionResult FMegalithUIBindingActions::HandleGetWidgetBindings(const TSharedPtr<FJsonObject>& Params)
{
    FString AssetPath = Params->GetStringField(TEXT("asset_path"));

    FMegalithActionResult Err;
    UWidgetBlueprint* WBP = MegalithUIInternal::LoadWidgetBlueprint(AssetPath, Err);
    if (!WBP) return Err;

    TArray<TSharedPtr<FJsonValue>> BindingsArray;

    for (const FDelegateEditorBinding& Binding : WBP->Bindings)
    {
        TSharedPtr<FJsonObject> BindObj = MakeShared<FJsonObject>();
        BindObj->SetStringField(TEXT("widget_name"), Binding.ObjectName);
        BindObj->SetStringField(TEXT("property_name"), Binding.PropertyName.ToString());
        BindObj->SetStringField(TEXT("function_name"), Binding.FunctionName.ToString());
        BindObj->SetStringField(TEXT("source_property"), Binding.SourceProperty.ToString());
        // SourcePath is FEditorPropertyPath, not FString — skip serialization

        BindingsArray.Add(MakeShared<FJsonValueObject>(BindObj));
    }

    TSharedPtr<FJsonObject> Result = MakeShared<FJsonObject>();
    Result->SetStringField(TEXT("asset_path"), AssetPath);
    Result->SetArrayField(TEXT("bindings"), BindingsArray);
    Result->SetNumberField(TEXT("count"), BindingsArray.Num());
    return FMegalithActionResult::Success(Result);
}
