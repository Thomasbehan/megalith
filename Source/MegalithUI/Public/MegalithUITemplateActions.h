// MegalithUITemplateActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUITemplateActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleCreateHudElement(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateMenu(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateSettingsPanel(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateDialog(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateNotificationToast(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateLoadingScreen(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateInventoryGrid(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleCreateSaveSlotList(const TSharedPtr<FJsonObject>& Params);
};
