#pragma once
#include "CoreMinimal.h"
#include "MegalithToolRegistry.h"

/**
 * DataTable dataset-ergonomics actions (Part B).
 *
 * All actions live in the "blueprint" namespace beside the existing
 * create_data_table / add_data_table_row / get_data_table_rows family. Each is
 * engine-generic: row structs resolve by string, and schema/type handling is
 * delegated to FMegalithReflectionWalker (MegalithCore) so this file never
 * reinvents reflection. Game-thread only.
 */
class FMegalithBlueprintDataTableActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

	// Read + schema
	static FMegalithActionResult HandleReadDataTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDescribeDataTableSchema(const TSharedPtr<FJsonObject>& Params);

	// Bulk upsert/add/update (dry_run + strict, FDryRunReport-shaped per field)
	static FMegalithActionResult HandleSetDataTableRows(const TSharedPtr<FJsonObject>& Params);

	// Row CRUD
	static FMegalithActionResult HandleRemoveDataTableRow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRenameDataTableRow(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateDataTableRow(const TSharedPtr<FJsonObject>& Params);

	// Whole-table JSON/CSV round-trip
	static FMegalithActionResult HandleExportDataTable(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleImportDataTable(const TSharedPtr<FJsonObject>& Params);
};
