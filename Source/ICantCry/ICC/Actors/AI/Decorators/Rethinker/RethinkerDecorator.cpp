// Fill out your copyright notice in the Description page of Project Settings.


#include "RethinkerDecorator.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

URethinkerDecorator::URethinkerDecorator()
{
	NodeName = TEXT("RethinkerCond");
}

bool URethinkerDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	checkf(BB, TEXT("Blackboard component is invalid URethinkerDecorator"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Invalid AIOwner"));
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Invalid AMob"));
	const bool bRethinker = BB->GetValueAsBool("Rethinker");
	return bRethinker;
}
