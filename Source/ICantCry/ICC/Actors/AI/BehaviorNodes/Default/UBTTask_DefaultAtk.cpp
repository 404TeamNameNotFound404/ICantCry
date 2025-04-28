#include "UBTTask_DefaultAtk.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "Navigation/PathFollowingComponent.h"
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
	BlackBoard->SetValueAsInt("Id", 0);
	BlackBoard->SetValueAsBool("Attacked?", false);
	
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	checkf(Controller, TEXT("AI Controller is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));
	
	checkf(Target, TEXT("Target is invalid at EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask"));

	bool Attacked = BlackBoard->GetValueAsBool("Attacked?");
	AMob* Current = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
	checkf(Current, TEXT("Current is invalid at Type UUBTTask_DefaultAtk::ExecuteTask"));
	
	Controller->MoveToActor(Target);
	Current->GetBattleHandler()->GetBattleInfo()->SetInfo(FText::FromString(Current->GetActorLabel() + " is attacking"));
	
	return EBTNodeResult::InProgress;
}

void UUBTTask_DefaultAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	
	if (FVector::Distance(Target->GetActorLocation(),Controller->GetPawn()->GetActorLocation()) <= 95.f && !bBusy)
	{
		Target->GetMinigameHandler()->StartMinigame(false);
		bBusy = true;
	}
	
	//
	if (!AMob::IsMinigameStarted() && AMob::MinigameEnded && Target->GetMinigameHandler()->IsPlayerMinigameEnded())
	{
		BlackBoard->SetValueAsInt("Id", -1); 
		bBusy = false;
		Cast<AMob>(Controller->GetPawn())->GetBattleHandler()->GetBattleInfo()->ClearInfo();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
