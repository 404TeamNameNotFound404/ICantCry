// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_BuffOtherAtk.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

UBTTask_BuffOtherAtk::UBTTask_BuffOtherAtk()
{
	NodeName = "TBuffOtherAtk";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_BuffOtherAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));


	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetBuffOtherAtk(true);
	Current->SetIsAttacked(false);
	
	BlackBoard->SetValueAsBool("IsBuffOtherAtk?", Current->GetBuffOtherAtk());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());

	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().CantBuffOthers())
	{
		if (Current->GetMobType() == EMobType::MobJealousy)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: this mf -> " + Current->GetActorLabel() + " tried to debuff atk someone but it's alone! so let's attack instead!");
			UUBTTask_DefaultAtk::GetInstance()->StartAttackMinigame(Current, Target, Controller);
			return EBTNodeResult::InProgress;
		}
		
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: " + Current->GetActorLabel() + " attempted to buff other atk but it's alone! , rethink the action");
		BlackBoard->SetValueAsBool("Rethinker", true);
		return EBTNodeResult::Succeeded;
	}

	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue(Current);
	if (!TargetToBuff) // rethink if target buff appears to be nullptr again 
	{
		if (Current->GetMobType() == EMobType::MobJealousy)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: this mf -> " + Current->GetActorLabel() + " tried to debuff atk someone but it's alone! so let's attack instead!");
			UUBTTask_DefaultAtk::GetInstance()->StartAttackMinigame(Current, Target, Controller);
			return EBTNodeResult::InProgress;
		}
		
		BlackBoard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: " + Current->GetActorLabel() + " attempted to buff other def but it didn't find a valid target (scrivetemelo se entra qua dentro)! , rethinking the action");
		return EBTNodeResult::Succeeded;
	}


	// check first if 'TargetToBuff' has active buff , if so it removes it to apply the new one
	//TargetToBuff->GetStatusTracker()->BuffFlow(EBuffStatus::AtkBuff, TargetToBuff);
	TargetToBuff->GetStatusTracker()->BuffWith(EBuffStatus::AtkBuff);
	

	DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: " + Current->GetActorLabel() + " buffed " + TargetToBuff->GetActorLabel() + " atk");

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " buffs " + TargetToBuff->GetActorLabel() + " atk"));
	
	return EBTNodeResult::InProgress;
}

void UBTTask_BuffOtherAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTimerStarted)
		return;

	bTimerStarted = true;

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());

	if (!Current || !Controller)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Current != Current->GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not " + Current->GetActorLabel() + "'s turn yet (buff def task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("buff atk target task completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Atk]: buff atk target task completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_BuffOtherAtk::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	

	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetBuffOtherAtk(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (BlackBoard)
		{
			BlackBoard->SetValueAsBool("IsBuffOtherAtk?", Current->GetBuffOtherAtk());
			BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
		}
		
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}

	bTimerStarted = false;
}
