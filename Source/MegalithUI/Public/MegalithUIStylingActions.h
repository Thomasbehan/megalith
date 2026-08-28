// MegalithUIStylingActions.h
#pragma once

#include "MegalithToolRegistry.h"

class FMegalithUIStylingActions
{
public:
    static void RegisterActions(FMegalithToolRegistry& Registry);

    static FMegalithActionResult HandleSetBrush(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetFont(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetColorScheme(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleBatchStyle(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetText(const TSharedPtr<FJsonObject>& Params);
    static FMegalithActionResult HandleSetImage(const TSharedPtr<FJsonObject>& Params);
};
