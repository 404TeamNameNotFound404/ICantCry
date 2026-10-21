// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Flee.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

UBTTask_Flee::UBTTask_Flee()
{
	NodeName = "Flee";
	bCreateNodeInstance = true;
	bNotifyTaskFinished = true;
}


EBTNodeResult::Type UBTTask_Flee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerRef = &OwnerComp;
	
	FBTFleeTaskMemory* MyMemory = CastInstanceNodeMemory<FBTFleeTaskMemory>(NodeMemory);
    if (!MyMemory) return EBTNodeResult::Failed;

	Blackboard = OwnerComp.GetBlackboardComponent();
    const AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
    Current = Cast<AMob>(Controller ? Controller->GetPawn() : nullptr);
    const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

    if (!Current || !Instance) return EBTNodeResult::Failed;

    UTurnBasedSystem* TBS = Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem();
	
    if (const bool bIsAlone = TBS->GetTurn().LastMobStanding() || TBS->GetTurn().AreAnxietyOrCalmAlone(); !bIsAlone)
    {
       if (Current->GetMobType() == EMobType::MobAnxiety)
       {
          Current->SetPlayerDebuffAttack(true);
          Blackboard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
          return EBTNodeResult::Succeeded;
       }
    	if (Current->GetMobType() == EMobType::MobCalm)
       {
          Current->SetBuffedDefence(true);
          Blackboard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
          return EBTNodeResult::Succeeded;
       }
       return EBTNodeResult::Failed;
    }
	
    MyMemory->Attempts++;
	int32 BBAttempts = Blackboard->GetValueAsInt("FleeAttempts") + 1;
	Blackboard->SetValueAsInt("FleeAttempts", BBAttempts);

    int32 SuccessRate = 0;
    if (BBAttempts == 1)      SuccessRate = 33;
    else if (BBAttempts == 2) SuccessRate = 66;
    else SuccessRate = 100;

    DebugHelper::AddMessageToLog("[BT - Flee]: Attempt N: " + FString::FromInt(BBAttempts) + " | Rate: " + FString::FromInt(SuccessRate) + "%");
	
	if (const int32 DiceRoll = FMath::RandRange(1, 100);
		DiceRoll <= SuccessRate)
	{
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
		   FText::FromString(Current->GetData()->EnemyName.ToString() + " ran away"));
		
		UBehaviorTreeComponent* OwnerCompPtr = &OwnerComp;

		FTimerHandle Delay;
		GetWorld()->GetTimerManager().SetTimer(Delay, [this, OwnerCompPtr]()
		{
		   Run();
		}, 1.5f, false);
	}
	else
	{
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
		   FText::FromString(Current->GetData()->EnemyName.ToString() + " tried to escape but failed"));

		UBehaviorTreeComponent* OwnerCompPtr = &OwnerComp;

		FTimerHandle Delay;
		GetWorld()->GetTimerManager().SetTimer(Delay, [this, OwnerCompPtr]()
		{
		   FailFlee();
		}, 1.85f, false);
	}
    
    return EBTNodeResult::InProgress;
}

void UBTTask_Flee::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	Current->SetFleeing(false);
	Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());

	FTimerHandle Delay;
	GetWorld()->GetTimerManager().SetTimer(Delay, [this]
	{
		const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}, 1.25f, false);
}

uint16 UBTTask_Flee::GetInstanceMemorySize() const
{
	return sizeof(FBTFleeTaskMemory);
}

void UBTTask_Flee::Run()
{
	const UICantCryGameInstance* Instance =
		Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	Instance->GetCurrentPlayer()
	        ->GetBattleHUD()
	        ->DecisionDisplayer
	        ->Hide();
	
	Current->GetBattleHandler()->GetTurnBasedSystem()->FleeAttempt(Current);
	Current->GetBattleHandler()->GetTurnBasedSystem()->EndTurn();
	Current->GetBattleHandler()->GetTurnBasedSystem()->StartNextTurn();
	Current->SetFleeing(false);
	Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());
	Blackboard->SetValueAsInt("FleeAttempts", 0);
	FinishLatentTask(*OwnerRef, EBTNodeResult::Succeeded);
}

void UBTTask_Flee::FailFlee()
{
	if (const UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer())
	{
		Instance->GetCurrentPlayer()
				->GetBattleHUD()
				->DecisionDisplayer
				->Hide();
	}
	
	Current->SetFleeing(false);
	Blackboard->SetValueAsBool("Flee?", Current->IsFleeing());
	Current->SetTreeId(-1);
	Blackboard->SetValueAsInt("Id", Current->GetTreeId());
	FinishLatentTask(*OwnerRef, EBTNodeResult::Succeeded);
	
	DebugHelper::LogMessage(10, FColor::Black, "Fail Fleeeeeee bruh dafaq");
}
