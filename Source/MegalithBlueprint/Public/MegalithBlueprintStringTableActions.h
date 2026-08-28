#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * StringTable dataset-ergonomics actions (Part B).
 *
 * Read/edit a UStringTable's key->source-string entries plus its namespace. All
 * actions live in the "blueprint" namespace. Engine-generic: the table resolves
 * by string via LoadAssetByPath/Cast.
 *
 * The editing surface lives on the wrapped FStringTable (Core module), reached
 * via UStringTable::GetMutableStringTable(). SetSourceString is natively an
 * upsert. Unlike CurveTable/DataTable there is NO editor-refresh broadcast for
 * StringTables — mutations just MarkPackageDirty + Modify (an open StringTable
 * tab may need a reselect to refresh). Game-thread only.
 */
class FMegalithBlueprintStringTableActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	static FMegalithActionResult HandleReadStringTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleSetStringTableEntries(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveStringTableEntry(const TSharedPtr<FJsonObject>& Params);
};
