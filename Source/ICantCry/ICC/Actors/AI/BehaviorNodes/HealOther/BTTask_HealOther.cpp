// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_HealOther.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/DebuffAtkTask/BTTask_DebuffAtk.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTTask_HealOther::UBTTask_HealOther()
{
	NodeName = "HealOther";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_HealOther::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_DebuffOtherShield::ExecuteTask"))
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current invalid atUBTTask_DebuffOtherShield::ExecuteTask"))

	Current->SetHealOther(true);
	Current->SetIsAttacked(false);
	
	Blackboard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
	Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	AICC_Player* Target = Cast<AICC_Player>(Blackboard->GetValueAsObject("Target"));
	checkf(Target, TEXT("Player invalid at UBTTask_DebuffOtherShield::ExecuteTask"))


	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().CantBuffOthers())
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " this mf can't heal other if it's alone so i'm attacking the main mf instead");
		
		UUBTTask_DefaultAtk::GetInstance()->StartAttackMinigame(Current, Target, Controller);
		
		return EBTNodeResult::InProgress;
	}
	
	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue(Current);
	if (!TargetToBuff) 
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " this mf can't heal other if the mf is invalid, so let's attack the main mf instead");
		UUBTTask_DefaultAtk::GetInstance()->StartAttackMinigame(Current, Target, Controller);
		return EBTNodeResult::Succeeded;
	}

	if (TargetToBuff->GetStats().Health >= TargetToBuff->GetData()->MaxHealth)
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " this mf can't heal other if it's full life.. Show no mercy to the main mf");
		UUBTTask_DefaultAtk::GetInstance()->StartAttackMinigame(Current, Target, Controller);
		return EBTNodeResult::InProgress;
	}
	else
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " this mf can heal other so here's the cure king!");
		Blackboard->SetValueAsBool("IsHealingOther?", true);
		return EBTNodeResult::Succeeded;
	}
	
	TargetToBuff->Heal(TargetToBuff->GetStats().Health *= 0.20f);
	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " heals " + TargetToBuff->GetActorLabel()));
	Target->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " heals " + TargetToBuff->GetActorLabel()));
	Target->GetBattleHUD()->DecisionDisplayer->Show();
	DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetActorLabel() + " heals " + TargetToBuff->GetActorLabel());
	
	return EBTNodeResult::InProgress;
}

void UBTTask_HealOther::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (bTimeStarted)
		return;
	
	bTimeStarted = true;

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Current = Cast<AMob>(Controller->GetPawn());

	if (!Current || !Controller)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (Current != Current->GetBattleHandler()->GetTurnBasedSystem()->TryGetCurrentPlayer()->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("Debuff Other Shield Task Completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: Debuff Other Shield Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_HealOther::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetHealOther(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (Blackboard)
		{
			Blackboard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
			Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		}

		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		
	}

	bTimeStarted = false;
}
