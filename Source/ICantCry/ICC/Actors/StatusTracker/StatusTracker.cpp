// Fill out your copyright notice in the Description page of Project Settings.
#include "StatusTracker.h"

#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


/**
 * -------------- REMINDERS ---------------
*
* Buffs and Debuffs affect two stats, both of the player and the enemy: Attack and Defense.
* A buff increases the targeted stat by 20% of its value. A debuff decreases the targeted stat by 20% of its value.
* Attack buffs and debuffs last for 3 Player Actions (Player) / Enemies Turns (Enemies)
* Defense buffs and debuffs last for 3 turns, both for the Player and the Enemies.
* Buffs and Debuffs do not stack on the same stat. When a stat already affected by a Buff or Debuff is targeted by another Buff or Debuff Action of the same type, the effect is not increased. Instead, the duration of the existing Buff or Debuff is reset. (Ex: if the Player whose Attack Buff ends in their next Player Action, uses an Attack Buff on themself, the current Attack Buff is removed and the newest Attack Buff is applied).
* When a stat that is affected by a Buff is targeted by a Debuff Action that targets the same stat, the Buff
* is removed, and vice versa. (Ex: If the Player, Attack Buffed, receives an Attack Debuff from an enemy,
* the Player’s Attack Buff is removed, and the Debuff doesn’t apply either.)

 */

// Sets default values for this component's properties
UStatusTracker::UStatusTracker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	bIsOwnerAfflicted = false;
}


// Called when the game starts
void UStatusTracker::BeginPlay()
{
	Super::BeginPlay();
	
}



bool UStatusTracker::IsAfflicted() const
{
	return bIsOwnerAfflicted;
}


void UStatusTracker::InflictStatus(const EAfflictedStatus& Status, AICC_Actor* Target)
{
	if (bIsOwnerAfflicted)
	{
		DebugHelper::LogError("A status has already been inflicted");
		return;
	}
	
	CurrentActiveStatus = Status;
	StatusCounter = 0;
	bIsOwnerAfflicted = true;
	
	switch (Status)
	{
	case Freezed:
		InflictFreeze(Target);
		break;
	case Burn:
		InflictBurn(Target);
		break;
	case EAShame:
		InflictAShamed(Target);
		break;
	case ShieldDebuff:
		InflictShieldDebuff(Target);
		break;
	default:
		break;
	}
}


void UStatusTracker::UpdateStatus()
{
	if (!bIsOwnerAfflicted)
	{
		return;
	}
	
	StatusCounter += 1;
	
	DebugHelper::LogWarning("Status Counter " + FString::FromInt(StatusCounter));

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (StatusCounter >= 3)
	{
		switch (CurrentActiveStatus)
		{
		case Freezed:
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Target->Freeze(false);
			break;
		case Burn:
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Target->Burn(false);
			break;
		case EAShame:
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Target->Ashamed(false);
			break;
		case ShieldDebuff:
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Target->ShieldDebuff(false);
			break;
		default:
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			break;
		}
	}
}

void UStatusTracker::UnfreezeChance()
{
	const float AleatoryChance = FMath::FRand();
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	DebugHelper::LogWarning("Attempting to auto freeze");

	if (constexpr float ChanceToFreeze = 0.25f; AleatoryChance <= ChanceToFreeze)
	{
		Target->Freeze(false);
		bIsOwnerAfflicted = false;
		CurrentActiveStatus = None;
		StatusCounter = 0;
	}
}

void UStatusTracker::BuffAttack() // TODO Status turn track
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->AttackPower += Player->GetStats()->AttackPower * 1.25f;
		DebugHelper::LogWarning("Attack buffed " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->AttackPower = Mob->GetData()->AttackPower * 1.25f;
	}
}

void UStatusTracker::InflictFreeze(AICC_Actor* Target)
{
	Target->Freeze(true);
}

void UStatusTracker::InflictBurn(AICC_Actor* Target)
{
	// no buff for the next 3 actions for both AI and player

	Target->Burn(true);
}

void UStatusTracker::InflictShieldDebuff(AICC_Actor* Target)
{
	// For 3 debuffs received by the enemy (player) /  Enemies turns (enemy), the target cannot be de-buffed

	Target->ShieldDebuff(true);
}

void UStatusTracker::InflictAShamed(AICC_Actor* Target)
{
	// AI can't target for attack

	Target->Ashamed(true);
	DebugHelper::LogMessage(5, FColor::FromHex("FE7743"), Target->GetActorLabel() + " can't perform attack");
	
}

