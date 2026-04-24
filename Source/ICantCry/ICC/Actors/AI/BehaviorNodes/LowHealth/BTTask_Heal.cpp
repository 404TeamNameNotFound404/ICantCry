#include "BTTask_Heal.h"

#include "BehaviorTree/BlackboardComponent.h"
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
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("Controller is invalid at heal execute task"));

	AMob* Current = Cast<AMob>(Controller->GetPawn());
	checkf(Current, TEXT("Current Mob is invalid heal execute task"));
	
	if (Current->GetStats().Health >= Current->GetData()->MaxHealth)
	{
		DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " Health is full ftw!");
		return EBTNodeResult::Succeeded;
	}
	
	DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " uses Heal");
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString(Current->GetActorLabel() + " uses heal"));
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
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

	Current->Heal(Current->GetTactics()->HealingPoint); // Can be edited via editor on the EnemyTactics data asset
	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " Healed ItSelf"));
	DebugHelper::AddMessageToLog("[Behavior Tree - Heal]: " + Current->GetActorLabel() + " Healed ItSelf");
	Timer = 0.0f;
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_Heal::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	
	if (AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner()))
	{
		AMob* Current = Cast<AMob>(Controller->GetPawn());

		Current->SetHeal(false);
		Current->SetIsAttacked(false);
		Current->SetTreeId(-1);

		if (OwnerComp.GetBlackboardComponent())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("IsHealing?", Current->GetIsHeal());
			OwnerComp.GetBlackboardComponent()->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
			OwnerComp.GetBlackboardComponent()->SetValueAsInt("Id", Current->GetTreeId());
		}

		Current->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}
}
