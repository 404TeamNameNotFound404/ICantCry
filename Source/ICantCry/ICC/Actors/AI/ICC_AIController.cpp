#include "ICC_AIController.h"
#include "Mob.h"


AICC_AIController::AICC_AIController(FObjectInitializer const& FObjectInitializer)
{
}

void AICC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (const AMob* Mob = Cast<AMob>(InPawn))
	{
		if (UBehaviorTree* BT = Mob->GetBehaviorTree())
		{
			UBlackboardComponent* BlackBoardCmp;
			UseBlackboard(BT->BlackboardAsset, BlackBoardCmp);
			Blackboard = BlackBoardCmp;
			RunBehaviorTree(BT);
		}
	}
}
