// Fill out your copyright notice in the Description page of Project Settings.
#include "BTTask_BuffOtherDef.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


UBTTask_BuffOtherDef::UBTTask_BuffOtherDef()
{
	NodeName = "BuffOtherDef";
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_BuffOtherDef::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetBuffOtherDefence(true);
	Current->SetIsAttacked(false);
	
	BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	
	if (Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().CantBuffOthers())
	{
		if (Current->GetMobType() == EMobType::MobFear)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: this mf " + Current->GetActorLabel() + " is a forever alone so its gonna buff it's def");
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", true);
			return EBTNodeResult::Succeeded;
		}
		
		else if (Current->GetMobType() == EMobType::MobCalm)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: this mf " + Current->GetActorLabel() + " is a forever alone so its gonna buff it's def");
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", true);
			return EBTNodeResult::Succeeded;
		}
		
		BlackBoard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog(Current->GetActorLabel() + " attempted to buff other def but it's alone! , rethink the action");
		return EBTNodeResult::Succeeded;
	}

	AMob* TargetToBuff = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue(Current);

	if (!TargetToBuff) // rethink if target buff appears to be nullptr again 
	{
		if (Current->GetMobType() == EMobType::MobFear)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: this mf " + Current->GetActorLabel() + " is a forever alone so its gonna buff it's def");
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", true);
			return EBTNodeResult::Succeeded;
		}
		
		else if (Current->GetMobType() == EMobType::MobCalm)
		{
			DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: this mf " + Current->GetActorLabel() + " is a forever alone so its gonna buff it's def");
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", true);
			return EBTNodeResult::Succeeded;
		}
		
		BlackBoard->SetValueAsBool("Rethinker", true);
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: " + Current->GetActorLabel() + " attempted to buff other def but it didn't find a valid target (scrivetemelo se entra qua dentro)! , rethinking the action");
		return EBTNodeResult::Succeeded;
	}
	

	TargetToBuff->GetStatusTracker()->BuffWith(EBuffStatus::DefBuff);

	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(
		FText::FromString(Current->GetActorLabel() + " buffed " + TargetToBuff->GetActorLabel() + " def"));

	DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: " + Current->GetActorLabel() + " buffed " + TargetToBuff->GetActorLabel() + " def");

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " buffs " + TargetToBuff->GetActorLabel() + " def"));
	
	return EBTNodeResult::InProgress;
}

void UBTTask_BuffOtherDef::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: It's not " + Current->GetActorLabel() + "'s turn yet (buff def task)");
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	TWeakObjectPtr<UBehaviorTreeComponent> OwnerCompWeak = &OwnerComp;

	Controller->GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this, OwnerCompWeak]()
	{
		if (!OwnerCompWeak.IsValid())
			return;

		DebugHelper::LogSuccess("buff def target task Task Completed");
		DebugHelper::AddMessageToLog("[Behavior Tree - Buff Other Def]: buff def target task Task Completed");
		FinishLatentTask(*OwnerCompWeak.Get(), EBTNodeResult::Succeeded);
	}), 1.0f, false);
}

void UBTTask_BuffOtherDef::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	

	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetBuffOtherDefence(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (BlackBoard)
		{
			BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
			BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
		}

		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}

	bTimerStarted = false;
}
