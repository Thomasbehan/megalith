// MegalithCommonUIActionsAggregator.cpp
// Central registration aggregator for all CommonUI categories. Each category file implements
// a namespace-scoped Register() function that this aggregator calls from StartupModule().
#if WITH_COMMONUI

#include "CommonUI/MegalithCommonUIActions.h"
#include "MegalithToolRegistry.h"

// Forward declarations — each category .cpp implements these.
namespace MegalithCommonUIActivatable    { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIButton         { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIInput          { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUINavigation     { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIList           { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIContent        { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIDialog         { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIAudit          { void Register(FMegalithToolRegistry&); }
namespace MegalithCommonUIAccessibility  { void Register(FMegalithToolRegistry&); }
// Phase 3 (2026-05-16 UI Gap Audit) — Tier-3 headline scaffolders
// (scaffold_main_menu / scaffold_settings_panel_with_tabs / scaffold_pause_menu).
namespace MegalithCommonUITemplate        { void Register(FMegalithToolRegistry&); }

void FMegalithCommonUIActions::RegisterAll(FMegalithToolRegistry& Registry)
{
	MegalithCommonUIActivatable::Register(Registry);
	MegalithCommonUIButton::Register(Registry);
	MegalithCommonUIInput::Register(Registry);
	MegalithCommonUINavigation::Register(Registry);
	MegalithCommonUIList::Register(Registry);
	MegalithCommonUIContent::Register(Registry);
	MegalithCommonUIDialog::Register(Registry);
	MegalithCommonUIAudit::Register(Registry);
	MegalithCommonUIAccessibility::Register(Registry);
	MegalithCommonUITemplate::Register(Registry);
}

#endif // WITH_COMMONUI
