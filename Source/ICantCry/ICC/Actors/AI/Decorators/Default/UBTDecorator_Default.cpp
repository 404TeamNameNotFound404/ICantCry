#include "UBTDecorator_Default.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UUBTDecorator_Default::UUBTDecorator_Default()
{
	NodeName = TEXT("Default");
}

bool UUBTDecorator_Default::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	return HasAnyStateEnabled(OwnerComp);
}

bool UUBTDecorator_Default::HasAnyStateEnabled(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	checkf(BB, TEXT("Blackboard component is invalid in UUBTDecorator_Default::AreAllStatesDisabled"));

	bool bIsBuffed = BB->GetValueAsBool(FName("IsBuffed?"));
	bool bIsDebuffed = BB->GetValueAsBool(FName("IsDebuffed?"));
	bool bIsAlive = BB->GetValueAsBool(FName("IsAlive?"));
	bool bEnvyBurn = BB->GetValueAsBool(FName("IsEnvyBurnedState?"));
	bool bAshamed = BB->GetValueAsBool(FName("IsAshamedState?"));
	
	return !bIsBuffed && !bIsDebuffed && bIsAlive && !bEnvyBurn && !bAshamed;
}
