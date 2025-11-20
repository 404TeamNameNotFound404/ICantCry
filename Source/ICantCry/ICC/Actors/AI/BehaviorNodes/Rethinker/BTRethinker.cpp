// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRethinker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTRethinker::UBTRethinker()
{
	NodeName = TEXT("Rethinker");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTRethinker::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Self = Cast<AMob>(BlackBoard->GetValueAsObject("SelfActor"));

	FDecisionMaker DecisionMaker;
	DecisionMaker.Setup(Self);
	DecisionMaker.Thought();

	DebugHelper::LogMessage(5,FColor::Orange,"Rethinking action");
	DebugHelper::AddMessageToLog(Self->GetData()->EnemyName.ToString() + " can't buff other .. rethinking a new action");

	return EBTNodeResult::Succeeded;
}
