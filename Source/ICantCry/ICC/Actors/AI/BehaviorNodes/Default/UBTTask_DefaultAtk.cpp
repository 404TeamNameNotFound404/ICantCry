#include "UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UUBTTask_DefaultAtk* UUBTTask_DefaultAtk::Instance;

UUBTTask_DefaultAtk::UUBTTask_DefaultAtk()
{
	NodeName = TEXT("NoStatusAttack");
	bNotifyTick = true;
	bCreateNodeInstance = true;
}

UUBTTask_DefaultAtk* UUBTTask_DefaultAtk::GetInstance()
{
	return Instance;
}

EBTNodeResult::Type UUBTTask_DefaultAtk::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Instance = this;

	BlackBoard = OwnerComp.GetBlackboardComponent();
	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	AMob* Self = Cast<AMob>(BlackBoard->GetValueAsObject("SelfActor"));
	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	checkf(Controller && Self && Target, TEXT("Invalid references in DefaultAtk task"));

	CurrentMob = Target->GetBattleHUD()->GetCurrentPlayingEmotion();
	checkf(CurrentMob, TEXT("CurrentMob is invalid at ExecuteTask"));

	
	CurrentMob->SetTreeId(0);
	CurrentMob->SetIsAttacked(false);
	BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
	BlackBoard->SetValueAsBool("Attacked?", CurrentMob->GetIsIsAttacked());

	// if (CurrentMob->IsAshamed() && CurrentMob->GetMobType() != EMobType::MobShame) // for the moment I just ignore shame because it's the only mob that attack only to prevent crash
	// {
	// 	DebugHelper::AddMessageToLog("[BT Task - Default]: " + CurrentMob->GetActorLabel() + " is ashamed, rethinking it's action");
	// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	// 	return EBTNodeResult::Succeeded;
	// }
	
	bWaitingForThinkCompletion = true;
	bBusy = true;

	DecisionMaker.Clear();
	DecisionMaker.Setup(CurrentMob);
	Decision = DecisionMaker.Thought();

	Target->GetBattleHUD()->DecisionDisplayer->Show();
	Target->GetBattleHUD()->DecisionDisplayer->SetDecisionText(
		FText::FromString(Self->GetData()->EnemyName.ToString() + " is thinking...")
	);

	Controller->GetWorld()->GetTimerManager().ClearTimer(DelayHandle);

	if (Delay > 0.f)
	{
		Controller->GetWorld()->GetTimerManager().SetTimer(
			DelayHandle,
			this,
			&UUBTTask_DefaultAtk::OnThinkComplete_Internal,
			Delay,
			false
		);
	}
	else
	{
		OnThinkComplete_Internal();
	}

	TreeComp = &OwnerComp;
	return EBTNodeResult::InProgress;
}

void UUBTTask_DefaultAtk::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bWaitingForThinkCompletion || !bBusy)
		return;

	AICC_AIController* Controller = Cast<AICC_AIController>(OwnerComp.GetAIOwner());
	if (!Controller || !CurrentMob || !BlackBoard)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	// if (!CurrentMob->IsMinigameStarted() || !CurrentMob->IsMinigameEnded() || !Target->GetMinigameHandler()->
	// 	IsPlayerMinigameEnded())
	// 	return;

	if (!(CurrentMob->IsMinigameStarted() && CurrentMob->IsMinigameEnded() &&
	  Target->GetMinigameHandler()->IsPlayerMinigameEnded()))
	{
		return;
	}

	
	CurrentMob->SetTreeId(-1);
	CurrentMob->SetIsBuffedAtk(false);
	BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
	BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
	BlackBoard->SetValueAsBool("IsDefenceDebuffed?", CurrentMob->GetIsTargetDefenceDebuffed());
	

	bBusy = false;
	bWaitingForThinkCompletion = false;

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UUBTTask_DefaultAtk::OnThinkComplete(UBehaviorTreeComponent* OwnerComp, AICC_AIController* Controller)
{
	if (!OwnerComp || !Controller || !CurrentMob || !BlackBoard)
		return;

	bWaitingForThinkCompletion = true;

	AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target"));
	if (!Target)
	{
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	if (Decision != EDecision::None)
	{
		ProcessDecision(Decision, CurrentMob, BlackBoard, OwnerComp, Target);
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	if (CurrentMob->IsAshamed() && Decision == EDecision::None)
	{
		DebugHelper::AddMessageToLog("[Behavior Tree]: " + CurrentMob->GetActorLabel() + " under ashamed state, can't attack so skip its turn");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	
	StartAttackMinigame(CurrentMob, Target, Controller);
}

void UUBTTask_DefaultAtk::OnThinkComplete_Internal()
{
	AICC_AIController* Controller = Cast<AICC_AIController>(CurrentMob->GetController());
	if (!Controller || !BlackBoard) return;

	UBehaviorTreeComponent* OwnerComp = Cast<UBehaviorTreeComponent>(Controller->BrainComponent);
	if (!OwnerComp) return;

	OnThinkComplete(OwnerComp, Controller);
}

void UUBTTask_DefaultAtk::StartAttackMinigame(AMob* Current, AICC_Player* Target, AICC_AIController* Controller)
{
	if (!Current || !Target || !Controller) return;

	Controller->MoveToActor(Target);


	DebugHelper::AddMessageToLog("[Behavior Tree]: " + Current->GetActorLabel() + " is attacking");

	Current->GetAIMemory().AttackLocation = Current->GetActorLocation();
	Target->GetBattleHUD()->DecisionDisplayer->Hide();
	Target->GetMinigameHandler()->StartMinigame(nullptr,false);
	Target->GetMinigameHandler()->OnMinigameEnded.AddDynamic(this, &UUBTTask_DefaultAtk::OnMinigameEndedCallback);
	bBusy = true;
}


void UUBTTask_DefaultAtk::ProcessDecision(EDecision Dec, AMob* Current, UBlackboardComponent* Board,
                                          UBehaviorTreeComponent* OwnerComp, AICC_Player* Target)
{
	AICC_AIController* Controller = Cast<AICC_AIController>(Current->GetController());
	if (!Controller || !Current || !BlackBoard || !OwnerComp || !Target)
		return;

	if (Current != Target->GetBattleHUD()->GetCurrentPlayingEmotion())
	{
		DebugHelper::LogMessage(7, FColor::FromHex("C68EFD"), "It's not the " + Current->GetActorLabel() + " turn yet");
		DebugHelper::AddMessageToLog("[Behavior Tree]: It's not the " + Current->GetActorLabel() + " turn yet");
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
		return;
	}


	switch (Dec)
	{
	case EDecision::BuffItSelf:
		if (Current->IsEAnger())
		{
			Current->SetIsBuffedAtk(true);
			BlackBoard->SetValueAsBool("IsBuffed?", Current->GetIsIsBuffedAtk());
		}
		break;

	case EDecision::HealItSelf:
		if (Current->IsHealer())
		{
			Current->SetHeal(true);
			BlackBoard->SetValueAsBool("IsHealing?", Current->GetIsHeal());
		}
		break;

	case EDecision::HealOther:
		if (Current->IsHealer())
		{
			Current->SetHealOther(true);
			BlackBoard->SetValueAsBool("IsHealingOther?", Current->GetIsHealOther());
		}
		break;

	case EDecision::DebuffDefence:
		if (Current->IsESadness() || Current->IsEAnxiety())
		{
			Current->SetIsTargetDefDebuffed(true);
			BlackBoard->SetValueAsBool("IsDefenceDebuffed?", Current->GetIsTargetDefenceDebuffed());
		}
		break;

	case EDecision::BuffDefence:
		if (Current->IsEFear() || Current->IsECalm())
		{
			Current->SetBuffedDefence(true);
			BlackBoard->SetValueAsBool("IsDefenceBuffed?", Current->GetIsBuffedDefence());
		}
		break;

	case EDecision::BuffOtherDefence:
		if (Current->IsEFear() || Current->IsECalm())
		{
			Current->SetBuffOtherDefence(true);
			BlackBoard->SetValueAsBool("IsBuffedOtherDef?", Current->GetBuffOtherDefence());
		}
		break;

	case EDecision::DebuffAtk:
		if (Current->IsEDisgust() || Current->IsEAnxiety())
		{
			Current->SetPlayerDebuffAttack(true);
			BlackBoard->SetValueAsBool("IsAttackDebuffed?", Current->GetPlayerDebuffAttack());
		}
		break;

	case EDecision::FreezedUp:
		if (Current->IsEAnxiety())
		{
			Current->SetIsFreezedUp(true);
			BlackBoard->SetValueAsBool("IsFreezedUp?", Current->GetIsIsFreezedUp());
		}
		break;

	case EDecision::BuffOther:
		if (Current->IsEJealousy())
		{
			Current->SetBuffOtherAtk(true);
			BlackBoard->SetValueAsBool("IsBuffOtherAtk?", Current->GetBuffOtherAtk());
		}
		break;

	case EDecision::EnvyBurned:
		if (Current->IsEJealousy())
		{
			Current->SetIsEnvyBurned(true);
			BlackBoard->SetValueAsBool("IsEnvyBurnedState?", Current->GetIsIsEnvyBurned());
		}
		break;

	case EDecision::DebuffShieldItSelf:
		if (Current->IsECalm())
		{
			Current->SetDebuffShield(true);
			BlackBoard->SetValueAsBool("IsShieldDebuffed?", Current->GetIsDebuffShield());
		}
		break;

	case EDecision::DebuffShieldOther:
		if (Current->IsECalm())
		{
			Current->SetDebuffOtherShield(true);
			BlackBoard->SetValueAsBool("IsOtherShieldDebuffed?", Current->GetIsDebuffOtherShield());
		}
		break;
	case EDecision::Low:
		if (Current->IsEShame())
		{
			return;
		}

		Current->GetStatusTracker()->InflictStatus(EAfflictedStatus::CriticHealth, Current);
		
		break;

	case EDecision::Flee:
		if (Current->IsEAnxiety() || Current->IsECalm())
		{
			Current->SetFleeing(true);
			BlackBoard->SetValueAsBool("Flee?", true);
			break;
		}
		
		break;
		
	case EDecision::None:
	default:
		StartAttackMinigame(Current, Target, Controller);
		return;
	}
	
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}

void UUBTTask_DefaultAtk::OnMinigameEndedCallback()
{
	if (!CurrentMob || !BlackBoard) return;

	if (AICC_Player* Target = Cast<AICC_Player>(BlackBoard->GetValueAsObject("Target")))
	{
		Target->GetMinigameHandler()->OnMinigameEnded.
		        RemoveDynamic(this, &UUBTTask_DefaultAtk::OnMinigameEndedCallback);
	}
	
	CurrentMob->SetTreeId(-1);
	CurrentMob->SetIsBuffedAtk(false);
	BlackBoard->SetValueAsInt("Id", CurrentMob->GetTreeId());
	BlackBoard->SetValueAsBool("IsBuffed?", CurrentMob->GetIsIsBuffedAtk());
	
	FinishLatentTask(*TreeComp, EBTNodeResult::Succeeded);
}
