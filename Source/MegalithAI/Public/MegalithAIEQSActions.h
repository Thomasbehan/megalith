#pragma once

#include "MegalithAIInternal.h"

class FMegalithAIEQSActions
{
public:
	static void RegisterActions(FMegalithToolRegistry& Registry);

private:
	// EQS CRUD
	static FMegalithActionResult HandleCreateEQSQuery(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleGetEQSQuery(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListEQSQueries(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDeleteEQSQuery(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleDuplicateEQSQuery(const TSharedPtr<FJsonObject>& Params);

	// Generators
	static FMegalithActionResult HandleAddEQSGenerator(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveEQSGenerator(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureEQSGenerator(const TSharedPtr<FJsonObject>& Params);

	// Tests
	static FMegalithActionResult HandleAddEQSTest(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleRemoveEQSTest(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureEQSTest(const TSharedPtr<FJsonObject>& Params);

	// Scoring & Filter
	static FMegalithActionResult HandleConfigureEQSScoring(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleConfigureEQSFilter(const TSharedPtr<FJsonObject>& Params);

	// Type enumeration
	static FMegalithActionResult HandleListEQSGeneratorTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListEQSTestTypes(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleListEQSContexts(const TSharedPtr<FJsonObject>& Params);

	// Validation
	static FMegalithActionResult HandleValidateEQSQuery(const TSharedPtr<FJsonObject>& Params);

	// Spec builder & templates
	static FMegalithActionResult HandleBuildEQSQueryFromSpec(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleReorderEQSTests(const TSharedPtr<FJsonObject>& Params);
	static FMegalithActionResult HandleCreateEQSFromTemplate(const TSharedPtr<FJsonObject>& Params);
};
