#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * CurveTable dataset-ergonomics actions (Part B).
 *
 * First CurveTable surface in Megalith — closes the read/edit/CRUD gap. All
 * actions live in the "blueprint" namespace beside the DataTable family. Each is
 * engine-generic: the table resolves by string via LoadAssetByPath/Cast.
 *
 * CurveTable rows are NOT FProperty-walkable: each row is an FRealCurve*
 * (concretely FRichCurve* or FSimpleCurve* per GetCurveTableMode()) stored in
 * UCurveTable::RowMap (TMap<FName, FRealCurve*>). So key (de)serialization is
 * bespoke, NOT a reflection-walker call.
 *
 * Mode-lock gotcha: a fresh UCurveTable is ECurveTableMode::Empty; the FIRST
 * AddRichCurve/AddSimpleCurve permanently locks rich-vs-simple. Mixed writes are
 * rejected with a clear error. Game-thread only.
 */
class FMegalithBlueprintCurveTableActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// Read keys (branches on rich/simple mode)
	static FMegalithActionResult HandleReadCurveTable(const TSharedPtr<FJsonObject>& Params);

	// Write keys (replace/merge; rich-vs-simple chosen from interp_mode, mode-lock honoured)
	static FMegalithActionResult HandleSetCurveTableKeys(const TSharedPtr<FJsonObject>& Params);

	// Row CRUD
	static FMegalithActionResult HandleAddCurveTableRow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveCurveTableRow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameCurveTableRow(const TSharedPtr<FJsonObject>& Params);
};
