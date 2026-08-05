// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_SelfHeal.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

class AMob;

UBTTask_SelfHeal::UBTTask_SelfHeal()
{
	NodeName = "SelfHeal";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_SelfHeal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_DebuffOtherShield::ExecuteTask"))
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current invalid atUBTTask_DebuffOtherShield::ExecuteTask"))

	Current->SetHeal(true);
	Current->SetIsAttacked(false);
	
	Blackboard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
	Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	Current->GetStatusTracker()->BuffWith(EBuffStatus::LowHealth);

	if (Current->GetStats().Health >= Current->GetData()->MaxHealth)
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - HealOther]: " + Current->GetEmotionName() + " casted heal on itself but heal is full ... rethinking");
		Blackboard->SetValueAsBool("Rethinker", true);
		return EBTNodeResult::Succeeded;
	}
	
	Current->Heal(Current->GetStats().Health*= 0.20f);
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(Current->GetWorld()->GetGameInstance());
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetEmotionName() + " uses Heal"));
	
	DebugHelper::AddMessageToLog("[Behavior Tree - Healing]: "+ Current->GetEmotionName() + " use Heal");
	
	return EBTNodeResult::InProgress;
}

void UBTTask_SelfHeal::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not " + Current->GetEmotionName() + "'s turn yet (buff task)");
		DebugHelper::AddMessageToLog("[Behavior Tree - Healing]: It's not " + Current->GetEmotionName() + "'s turn yet (buff task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("Debuff Other Shield Task Completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - Healing]: Self heal task completed!");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_SelfHeal::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetHeal(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (Blackboard)
		{
			Blackboard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
			Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		}
		
		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(Current->GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(""));
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
		
	}

	bTimeStarted = false;
}
