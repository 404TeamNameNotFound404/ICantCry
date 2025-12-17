#include "BTTask_Heal.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


UBTTask_Heal::UBTTask_Heal()
{
	NodeName = "Heal";
	bNotifyTick = true;
	BlackBoard = nullptr;
}

EBTNodeResult::Type UBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller is invalid at heal execute task"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current Mob is invalid heal execute task"));
	
	
	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " uses Heal"));
	DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " uses Heal");
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " uses heal"));
	FDecisionMaker DecisionMaker;
	DecisionMaker.DecisionMap.Add(EDecision::HealItSelf, 0.70f); // from 0.0 to 0.7 heal itself 70%
	DecisionMaker.DecisionMap.Add(EDecision::HealOther, 0.30f); // from 0.7 to 1.0 heal other  7%
	Decision = DecisionMaker.Thought();
	
	return EBTNodeResult::InProgress;
}


void UBTTask_Heal::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller is invalid at heal TickTask"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current Mob is invalid heal TickTask"));

	Timer += DeltaSeconds * 2.0f;

	if (Timer >= TimeToHeal)
	{
		if (Decision == EDecision::HealItSelf)
		{
			Current->Heal(Current->GetTactics()->HealingPoint); // Can be edited via editor on the EnemyTactics data asset
			Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " Healed ItSelf"));
			DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " Healed ItSelf");
			Timer = 0.0f;
			UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}

		if (Decision == EDecision::HealOther)
		{
			AMob* Other = Current->GetBattleHandler()->GetTurnBasedSystem()->GetTurn().GetMobInQueue();
			checkf(Other, TEXT("Other Mob is invalid heal TickTask"));
			Other->Heal(Current->GetTactics()->HealingPoint);
			Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " Healed " + Other->GetActorLabel()));
			DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " Healed " + Other->GetActorLabel());
			Timer = 0.0f;
			UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
}
