// MegalithEditorMapActions.h
// Phase F8 (J-phase) — empty map authoring + module/plugin reflection.
//
//   editor::create_empty_map       — Create a fully blank UWorld asset on disk
//                                    via UWorldFactory + IAssetTools::CreateAsset.
//   editor::get_module_status      — Reflect plugin enable + module load status
//                                    for Megalith (or arbitrary) modules. Wraps
//                                    IPluginManager::GetDiscoveredPlugins +
//                                    FModuleManager::IsModuleLoaded.
//
// Both actions are project-agnostic (no Leviathan-specific symbols) and live in
// the editor-only MegalithEditor module. Registration is invoked from
// FMegalithEditorModule::StartupModule via FMegalithEditorMapActions::RegisterActions.
#pragma once

#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

class FMegalithEditorMapActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleCreateEmptyMap(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetModuleStatus(const TSharedPtr<FJsonObject>& Params);
};
