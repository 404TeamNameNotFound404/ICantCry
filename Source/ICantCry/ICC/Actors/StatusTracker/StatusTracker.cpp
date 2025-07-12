// Fill out your copyright notice in the Description page of Project Settings.
#include "StatusTracker.h"

#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
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

bool UStatusTracker::IsBuffed() const
{
	return bIsOwnerAlreadyBuffed;
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

	DebugHelper::LogMessage(6, FColor::Black, "Inflicting a status to " + Target->GetActorLabel());
	
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

void UStatusTracker::BuffWith(const EBuffStatus& BuffStatus)
{
	if (bIsOwnerAlreadyBuffed || !bCanBuff)
	{
		return;
	}
	
	CurrentBuffedStatus = BuffStatus;
	BuffStatusCounter = 0;
	bIsOwnerAlreadyBuffed = true;

	switch (BuffStatus)
	{
	case AtkBuff:
		BuffAttack();
		break;
	case DefBuff:
		BuffDefence();
		break;
	case LowHealth:
		Heal();
		break;
	default:
		break;
	}
}


void UStatusTracker::UpdateStatus()
{
	if (!bIsOwnerAfflicted)
	{
		DebugHelper::LogError("No debuff status found");
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
			bCanBuff = true;
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

void UStatusTracker::UpdateBuffStatus()
{
	if (!bIsOwnerAlreadyBuffed || !bCanBuff)
	{
		DebugHelper::LogError("No buff status found");
		return;
	}

	BuffStatusCounter += 1;

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	DebugHelper::LogMessage(6, FColor::Blue, "Buff Status counter " + FString::FromInt(BuffStatusCounter));

	if (BuffStatusCounter < 3)
	{
		return; // I don't need to check further if counter is not 3 (3 turns elapsed)
	}

	switch (CurrentBuffedStatus)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(GetOwner());
			UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
			Player->GetStats()->AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(GetOwner());
			Emotion->GetData()->AttackPower = Emotion->GetAIMemory().InitialAttackPower;
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(GetOwner());
			UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
			Player->GetStats()->DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(GetOwner());
			Emotion->GetData()->DefencePower = Emotion->GetAIMemory().InitialDefencePower;
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
		}
		break;
	case LowHealth:
		BuffStatusCounter = 0;
		bIsOwnerAlreadyBuffed = false;
		break;
	default:
		break;
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

void UStatusTracker::BuffAttack()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->AttackPower += FMath::FloorToInt(Player->GetStats()->AttackPower * 1.25f);
		DebugHelper::LogWarning("Attack buffed " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetTactics()->MovePower = FMath::FloorToInt(Mob->GetTactics()->MovePower * 1.25f);
	}
}

void UStatusTracker::BuffDefence()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->DefencePower += FMath::FloorToInt(Player->GetStats()->DefencePower * 1.25f);
		DebugHelper::LogWarning("Defence buffed " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower = FMath::FloorToInt(Mob->GetData()->DefencePower * 1.25f);
	}
}

void UStatusTracker::Heal()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetBattleHUD()->RestoreHealth();
		bIsOwnerAlreadyBuffed = false;
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetHealthBar()->Restore(Mob->GetData()->AttackPower); // maybe add a RestorePower?
		bIsOwnerAlreadyBuffed = false;
	}
}

void UStatusTracker::InflictFreeze(AICC_Actor* Target)
{
	bIsOwnerAfflicted = true;
	Target->Freeze(true);
}

void UStatusTracker::InflictBurn(AICC_Actor* Target)
{
	// no buff for the next 3 actions for both AI and player
	bIsOwnerAfflicted = true;
	Target->Burn(true);
	bCanBuff = false;
	DebugHelper::LogWarning(Target->GetActorLabel() + " in envy burned state\nCan buff " + FString::FromInt(bCanBuff));
}

void UStatusTracker::InflictShieldDebuff(AICC_Actor* Target)
{
	// For 3 debuffs received by the enemy (player) /  Enemies turns (enemy), the target cannot be de-buffed
	bIsOwnerAfflicted = true;
	Target->ShieldDebuff(true);
}

void UStatusTracker::InflictAShamed(AICC_Actor* Target)
{
	// AI can't target for attack
	bIsOwnerAfflicted = true;
	Target->Ashamed(true);
	DebugHelper::LogMessage(5, FColor::FromHex("FE7743"), Target->GetActorLabel() + " can't perform attack");
	
}

