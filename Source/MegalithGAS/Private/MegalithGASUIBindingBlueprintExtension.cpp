// MegalithGASUIBindingBlueprintExtension.cpp

#include "MegalithGASUIBindingBlueprintExtension.h"
#include "MegalithGASAttributeBindingClassExtension.h"
#include "MegalithGASInternal.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "WidgetBlueprintCompiler.h"

// Phase J F9: file-static `LogMegalithGAS` retired in favor of the parent
// LogMegalithGAS category (declared in MegalithGASInternal.h, defined in MegalithGASModule.cpp).

int32 UMegalithGASUIBindingBlueprintExtension::IndexOfBinding(FName WidgetName, FName PropertyName) const
{
    for (int32 i = 0; i < Bindings.Num(); ++i)
    {
        if (Bindings[i].TargetWidgetName == WidgetName && Bindings[i].TargetPropertyName == PropertyName)
        {
            return i;
        }
    }
    return INDEX_NONE;
}

int32 UMegalithGASUIBindingBlueprintExtension::AddOrReplaceBinding(const FMegalithGASAttributeBindingSpec& Spec, bool bReplaceExisting, bool& bOutReplaced)
{
    bOutReplaced = false;
    int32 Existing = IndexOfBinding(Spec.TargetWidgetName, Spec.TargetPropertyName);
    if (Existing != INDEX_NONE)
    {
        if (!bReplaceExisting)
        {
            return INDEX_NONE;
        }
        Modify();
        Bindings[Existing] = Spec;
        bOutReplaced = true;
        return Existing;
    }
    Modify();
    return Bindings.Add(Spec);
}

bool UMegalithGASUIBindingBlueprintExtension::RemoveBinding(FName WidgetName, FName PropertyName)
{
    int32 Idx = IndexOfBinding(WidgetName, PropertyName);
    if (Idx == INDEX_NONE) return false;
    Modify();
    Bindings.RemoveAt(Idx);
    return true;
}

int32 UMegalithGASUIBindingBlueprintExtension::ClearBindings()
{
    int32 Count = Bindings.Num();
    if (Count > 0)
    {
        Modify();
        Bindings.Reset();
    }
    return Count;
}

void UMegalithGASUIBindingBlueprintExtension::HandleBeginCompilation(FWidgetBlueprintCompilerContext& InCreationContext)
{
    Super::HandleBeginCompilation(InCreationContext);
    CachedCompilerContext = &InCreationContext;
}

void UMegalithGASUIBindingBlueprintExtension::HandleFinishCompilingClass(UWidgetBlueprintGeneratedClass* Class)
{
    Super::HandleFinishCompilingClass(Class);

    if (!Class) return;
    if (Bindings.Num() == 0) return;

    // Mirrors UUIComponentWidgetBlueprintExtension::HandleFinishCompilingClass:
    // mint a fresh runtime extension and install via the official compiler API.
    UMegalithGASAttributeBindingClassExtension* RuntimeExt = NewObject<UMegalithGASAttributeBindingClassExtension>(Class);
    RuntimeExt->Bindings = Bindings;

    if (CachedCompilerContext)
    {
        CachedCompilerContext->AddExtension(Class, RuntimeExt);
        UE_LOG(LogMegalithGAS, Log,
            TEXT("[GASBind] Installed runtime extension on %s with %d bindings"),
            *Class->GetName(), Bindings.Num());
    }
    else
    {
        UE_LOG(LogMegalithGAS, Warning,
            TEXT("[GASBind] HandleFinishCompilingClass: CompilerContext was null, runtime extension not installed."));
    }
}

void UMegalithGASUIBindingBlueprintExtension::HandleEndCompilation()
{
    Super::HandleEndCompilation();
    CachedCompilerContext = nullptr;
}
