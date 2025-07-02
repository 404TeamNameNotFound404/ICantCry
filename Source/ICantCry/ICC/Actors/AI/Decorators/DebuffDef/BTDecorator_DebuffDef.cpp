#include "BTDecorator_DebuffDef.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


UBTDecorator_DebuffDef::UBTDecorator_DebuffDef()
{
	NodeName = "IsTargetDefenceDebuffed?";
}

bool UBTDecorator_DebuffDef::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	const bool bDeBuff = Blackboard->GetValueAsBool("IsDefenceDebuffed?");
	DebugHelper::LogError("De-buffer condition is met");

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());

	UE_LOG(LogTemp, Warning, TEXT("Debuff decorator correctly joined"));
	
	return bDeBuff;
}
