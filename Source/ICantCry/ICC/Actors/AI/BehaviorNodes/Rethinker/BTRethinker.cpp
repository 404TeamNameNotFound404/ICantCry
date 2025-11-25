// Fill out your copyright notice in the Description page of Project Settings.


#include "BTRethinker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/AI/DecisionMaker/DecisionMaker.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTRethinker::UBTRethinker()
{
	NodeName = TEXT("Rethinker");
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTRethinker::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	BlackBoard = OwnerComp.GetBlackboardComponent();
	const AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	Current = Cast<AMob>(Controller->GetPawn());
	
	DebugHelper::AddMessageToLog(Current->GetData()->EnemyName.ToString() + " can't buff other .. rethinking a new action");
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));

    Current->SetRethink(true);
    BlackBoard->SetValueAsBool("Rethinker", Current->GetRethink());
    
	DecisionMaker.Setup(Current);
	Decision = DecisionMaker.Thought();

	UUBTTask_DefaultAtk::GetInstance()->ProcessDecision(Decision, Current, BlackBoard, &OwnerComp, Target);

	Current->SetRethink(false);
	BlackBoard->SetValueAsBool("Rethinker", Current->GetRethink());
	
	return EBTNodeResult::InProgress;
}

void UBTRethinker::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (Current && BlackBoard)
	{
		Current->SetRethink(false);
		BlackBoard->SetValueAsBool("Rethinker", false);
	}
}
