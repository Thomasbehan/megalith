// Copyright Megalith. All Rights Reserved.
//
// Generic AIController that auto-starts an assigned BehaviorTree in OnPossess().

#include "Controllers/MegalithBehaviorTreeAIController.h"
#include "MegalithAIInternal.h"

#include "BehaviorTree/BehaviorTree.h"

void AMegalithBehaviorTreeAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeToRun)
	{
		StartBehaviorTree();
	}
}

bool AMegalithBehaviorTreeAIController::StartBehaviorTree()
{
	if (!BehaviorTreeToRun)
	{
		UE_LOG(LogMegalithAI, Verbose,
			TEXT("MegalithBehaviorTreeAIController[%s]: no BehaviorTreeToRun assigned"),
			*GetName());
		return false;
	}

	const bool bStarted = RunBehaviorTree(BehaviorTreeToRun);
	if (!bStarted)
	{
		UE_LOG(LogMegalithAI, Warning,
			TEXT("MegalithBehaviorTreeAIController[%s]: RunBehaviorTree failed for %s"),
			*GetName(), *BehaviorTreeToRun->GetName());
	}

	return bStarted;
}
