// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_DebuffOtherShield.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UBTTask_DebuffOtherShield::UBTTask_DebuffOtherShield()
{
	NodeName = "DebuffOtherShield";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_DebuffOtherShield::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Blackboard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller invalid at UBTTask_DebuffOtherShield::ExecuteTask"))
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current invalid atUBTTask_DebuffOtherShield::ExecuteTask"))

	Current->SetDebuffOtherShield(true);
	Current->SetIsAttacked(false);
	
	Blackboard->SetValueAsBool("IsOtherShieldDebuffed?", Current->GetIsDebuffOtherShield());
	Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	AICC_Player* Target = Cast<AICC_Player>(Blackboard->GetValueAsObject("Target"));
	checkf(Target, TEXT("Player invalid at UBTTask_DebuffOtherShield::ExecuteTask"))

	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().CantBuffOthers())
	{
		if (Current->GetMobType() == EMobType::MobCalm)
		{
			Blackboard->SetValueAsBool("IsShieldDebuffed?", true);
			DebugHelper::AddMessageToLog("[BTTask - Debuff Other Shield]: " + Current->GetActorLabel() + " couldn't cast it's shield to other mfs so it's gonna buff it's shield back again");
			return EBTNodeResult::Succeeded;
		}
		
		Blackboard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog(Current->GetActorLabel() + " attempted to buff other shield but it's alone! , rethink the action");
		return EBTNodeResult::Succeeded;
	}
	
	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue(Current);
	
	if (!TargetToBuff) // rethink if target buff appears to be nullptr again 
	{
		if (Current->GetMobType() == EMobType::MobCalm)
		{
			Blackboard->SetValueAsBool("IsShieldDebuffed?", true);
			DebugHelper::AddMessageToLog("[BTTask - Debuff Other Shield]: " + Current->GetActorLabel() + " couldn't cast it's shield to other mfs so it's gonna buff it's shield back again");
			return EBTNodeResult::Succeeded;
		}
		
		Blackboard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog("[Behavior Tree - DOS]: " + Current->GetActorLabel() + " attempted to buff other def but it didn't find a valid target (scrivetemelo se entra qua dentro)! , rethinking the action");
		return EBTNodeResult::Succeeded;
	}

	// TargetToBuff->GetStatusTracker()->BuffFlow(EBuffStatus::DefBuff, TargetToBuff);
	// TargetToBuff->GetStatusTracker()->BuffWith(EBuffStatus::Shield);
	TargetToBuff->GetStatusTracker()->InflictStatus(EAfflictedStatus::ShieldDebuff, TargetToBuff);

	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " de-buffed " + TargetToBuff->GetActorLabel() + " shield"));
	DebugHelper::AddMessageToLog("[Behavior Tree - DOS]: " + Current->GetActorLabel() + " de-buffed " + TargetToBuff->GetActorLabel() + " shield");
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " de-buffed " + TargetToBuff->GetActorLabel() + " shield"));
	
	return EBTNodeResult::InProgress;
}

void UBTTask_DebuffOtherShield::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		DebugHelper::AddMessageToLog("[Behavior Tree - DOS]: It's not " + Current->GetActorLabel() + "'s turn yet (buff task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("Debuff Other Shield Task Completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - DOS]: Debuff Other Shield Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_DebuffOtherShield::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetDebuffOtherShield(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (Blackboard)
		{
			Blackboard->SetValueAsBool("IsOtherShieldDebuffed?", Current->GetIsDebuffOtherShield());
			Blackboard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			Blackboard->SetValueAsInt("Id", Current->GetTreeId());
		}

		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}

	bTimeStarted = false;
}
