#include "UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UUBTTask_DefaultAtk::UUBTTask_DefaultAtk()
{
	NodeName = TEXT("NoStatusAttack");
	bNotifyTick = true;
	
}

EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BlackBoard = OwnerComp.GetBlackboardComponent();
	
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));
	
	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));
	

	bool Attacked = BlackBoard->GetValueAsBool("Attacked?");
	AMob* Current = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));

	Current->SetTreeId(0);
	Current->SetIsAttacked(false);
	BlackBoard->SetValueAsInt("Id", Current->GetTreeId());
	BlackBoard->SetValueAsBool("Attacked?", Current->GetIsIsAttacked());
	

	FDecisionMaker DecisionMaker;
	DecisionMaker.DecisionMap.Add(EDecision::BuffItSelf, 0.55);
	Decision = DecisionMaker.Thought();
	
	// Controller->MoveToActor(Target);
	// Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
	
	return EBTNodeResult::InProgress;
}

void UUBTTask_DefaultAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	AMob* Current = Cast<AMob>(Controller->GetPawn());
	
	
	if (Decision == EDecision::BuffItSelf)
	{
		if (Current->IsHealer())
		{
			return;
		}
			
		Current->SetIsBuffed(true);
		BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffed());
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	if (Decision == EDecision::None)
	{
		Controller->MoveToActor(Target);
		Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
	
		switch (Decision)
		{
		case EDecision::BuffItSelf:
			if (!Current->IsHealer())
			{
				Current->SetIsBuffed(true);
				BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffed());
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed); // Or pick something meaningful
			}
			break;

		case EDecision::None:
			if (FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < 95.f  && !bBusy) // FVector::Distance(Target->GetActorLocation(), Controller->GetPawn()->GetActorLocation()) < 98.f Original Threshold = 95 
			{
				Cast<AMob>(Controller->GetPawn())->GetAIMemory().AttackLocation = Cast<AMob>(Controller->GetPawn())->GetActorLocation();
				Target->GetMinigameHandler()->StartMinigame(false);
				bBusy = true;
			}
	

			if (!AMob::IsMinigameStarted() && AMob::MinigameEnded && Target->GetMinigameHandler()->IsPlayerMinigameEnded())
			{
				Current->SetTreeId(-1);
				Current->SetIsBuffed(false);
				BlackBoard->SetValueAsInt("Id", Current->GetTreeId()); 
				BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffed());
				bBusy = false;
				Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			// attack logic
			break;

		default:
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			break;
		}
	
	}
	
}
