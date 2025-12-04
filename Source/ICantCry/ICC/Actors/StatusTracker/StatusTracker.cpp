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


void FStatusPriority::SetNextPriorityFromBuff(const EBuffStatus& BuffStatus)
{
	DebugHelper::AddMessageToLog("Set next priority from buff for Current Emotion turn");
	switch (BuffStatus)
	{
	case AtkBuff:
		SetNextBuffProcessPriority(1);
		break;
	case DefBuff:
		SetNextBuffProcessPriority(2);
		break;
	case Shield:
		SetNextBuffProcessPriority(4);
		break;
	case LowHealth:
		SetNextBuffProcessPriority(3);
		break;
	default:
	case NoBuff:
		break;
	}
}

void FStatusPriority::SetNextPriorityFromDebuff(const EAfflictedStatus& Status)
{
	DebugHelper::AddMessageToLog("Set next priority from debuff for Current Emotion turn");
	switch (Status)
	{
	case Freezed:
		break;
	case Burn:
		SetNextPriority(4);
		break;
	case EAShame:
		SetNextPriority(4);
		break;
	case ShieldDebuff:
		SetNextPriority(4);
		break;
	case DebuffAtk:
		SetNextPriority(1);
		break;
	case DebuffDef:
		SetNextPriority(2);
		break;
	case CriticHealth:
		SetNextPriority(3);
		break;
	default:
	case None:
		SetNextPriority(0);
		break;
	}
}

void FInternalPerkData::CheckPriority(AMob* Mob)
{
	UStatusTracker* StatusTracker = Mob->GetStatusTracker();
	
	if (!StatusTracker->IsAfflicted() || !StatusTracker->IsBuffed())
	{
		return;
	}

	
}

bool FInternalPerkData::HasBuffHightPriority(AMob* Emotion) const
{
	return   Emotion->GetStatusTracker()->GetStatusPriority().GetNextBuffPriority() >
	Emotion->GetStatusTracker()->GetStatusPriority().GetBuffPriority();
}

bool FInternalPerkData::HasHighDebuffPriority(AMob* Emotion) const
{
	return Emotion->GetStatusTracker()->GetStatusPriority().GetNextPriority() >
	       Emotion->GetStatusTracker()->GetStatusPriority().GetPriority();
}

void FInternalPerkData::AssignPriority(AMob* Emotion)
{
	FInternalPerkData& Data = Emotion->GetStatusTracker()->GetPerkData();
	FStatusPriority StatusPriority = Emotion->GetStatusTracker()->GetStatusPriority();

	switch (StatusPriority.GetNextBuffPriority())
	{
	default:
	case 0:
		{
			Data.Clear();
			break;
		}

	case 1:
		{
			Data.Clear();
			if (Emotion->GetIsIsBuffedAtk())
			{
				Data.Clear();
				DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				Data.bBuffAtk = true;
			}

			else if (Emotion->GetPlayerDebuffAttack())
			{
				Data.Clear();
				DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				Data.bDebuffAtk = true;
			}
		}

	case 2:
		{
			Data.Clear();
			if (Emotion->GetIsBuffedDefence())
			{
				Data.Clear();
				DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				Data.bBuffDef = true;
			}

			else if (Emotion->GetIsTargetDefenceDebuffed())
			{
				Data.Clear();
				DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				Data.bDebuffDef = true;
			}
		}

	case 3:
		{
			Data.Clear();
			DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
			Data.bLowHealth = true;
		}

	case 4:
		{
			
		}
	}
}

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
	if (bIsOwnerAfflicted || !bCanDebuff)
	{
		DebugHelper::LogError("A status has already been inflicted");
		DebugHelper::AddMessageToLog("Emotion attempted to cast " + GetStatusName(Status) + " To " + Target->GetActorLabel() + " but " + GetStatusName(CurrentActiveStatus) + " has already been inflicted");
		return;
	}

	if (bShieldBuffed)
	{
		PlayerShieldAccumulator--;
		DebugHelper::AddMessageToLog(GetOwner()->GetActorLabel() + " attempted to inflict " + GetStatusName(Status) + " but fortunately a shield was used to protect from the debuff "
			+ FString::FromInt(PlayerShieldAccumulator) + " left");

		if (PlayerShieldAccumulator <= 0)
		{
			bShieldBuffed = false;
			PlayerShieldAccumulator = 0;
			DebugHelper::AddMessageToLog("Shield debuff protection for " + GetOwner()->GetActorLabel() + " ended");
		}
		else
		{
			return;
		}
	}

	CurrentActiveStatus = Status;
	StatusCounter = 0;
	bIsOwnerAfflicted = true;

	DebugHelper::LogMessage(6, FColor::Black, "Inflicting  " + GetStatusName(CurrentActiveStatus) + " To " + Target->GetActorLabel());
	DebugHelper::AddMessageToLog("Inflicting  " + GetStatusName(CurrentActiveStatus) + " To " + Target->GetActorLabel());

	switch (Status)
	{
	case Freezed:
		InflictFreeze(Target);
		break;
	case Burn:
		Priority.SetBuffCurrentPriority(4);
		InflictBurn(Target);
		break;
	case EAShame:
		Priority.SetBuffCurrentPriority(4);
		InflictAShamed(Target);
		break;
	case ShieldDebuff:
		Priority.SetBuffCurrentPriority(4);
		InflictShieldDebuff(Target);
		break;
	case DebuffAtk:
		Priority.SetBuffCurrentPriority(1);
		DebuffAtkF();
		break;
	case DebuffDef:
		Priority.SetBuffCurrentPriority(2);
		DebuffDefF();
		break;
	case CriticHealth:
		Priority.SetBuffCurrentPriority(3);
		if (GetOwner()->IsA(AMob::StaticClass()))
		{
			if (const AMob* Shame = Cast<AMob>(GetOwner()); Shame->IsEShame())
			{
				DebugHelper::AddMessageToLog("Mob is shame, can't go low health, skipping low health decision table");
				return;
			}
			
			DebugHelper::AddMessageToLog(GetOwner()->GetActorLabel() + " is in low health status!");
			PerkData.bLowHealth = true;
		}
	default:
	case None:
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
		Priority.SetPriotity(1);
		BuffAttack();
		break;
	case DefBuff:
		Priority.SetPriotity(2);
		BuffDefence();
		break;
	case LowHealth:
		Heal();
		break;
	case Shield:
		Priority.SetPriotity(4);
		BuffShield();
		break;
	default:
	case NoBuff:
		break;
	}
}


void UStatusTracker::UpdateStatus()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	if (Target->IsA(AMob::StaticClass()))
	{
		if (const AMob* Self = Cast<AMob>(GetOwner()); !Self->IsLowHealth())
		{
			DebugHelper::AddMessageToLog(Self->GetActorLabel() + " health is not low");
			PerkData.bLowHealth = false;
		}
	}
	
	if (!bIsOwnerAfflicted || !bCanDebuff)
	{
		DebugHelper::AddMessageToLog("No debuff status found for " + GetOwner()->GetActorLabel());
		return;
	}

	StatusCounter += 1;

	DebugHelper::LogWarning("Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	DebugHelper::AddMessageToLog("Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	

	if (StatusCounter < 3)
	{
		return;
	}

	RevertInflictedMalus(CurrentActiveStatus);
	
	switch (CurrentActiveStatus)
	{
	case Freezed:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Target->Freeze(false);

		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bFreezedUp = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		
		break;
	case Burn:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Target->Burn(false);
		bCanBuff = true;
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bEnvyBurned = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		break;
	case EAShame:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Target->Ashamed(false);
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bAshamed = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		break;
	case ShieldDebuff:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Target->ShieldDebuff(false);
		bCanDebuff = true;
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bShieldDebuff = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		break;
	case DebuffAtk:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bDebuffAtk = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		break;
	case DebuffDef:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bDebuffDef = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		break;
	default:
	case None:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Priority.SetPriotity(0);
		break;
	}
}

void UStatusTracker::UpdateBuffStatus()
{
	if (!bIsOwnerAlreadyBuffed || !bCanBuff)
	{
		DebugHelper::AddMessageToLog("No buff status found for " + GetOwner()->GetActorLabel());
		return;
	}

	BuffStatusCounter += 1;

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	DebugHelper::AddMessageToLog(Target->GetActorLabel() + " Buff Status counter of " + GetBuffName(CurrentBuffedStatus) + " " + FString::FromInt(BuffStatusCounter));

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
			DebugHelper::AddMessageToLog("Buff ended atk returns to " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(GetOwner());
			Emotion->GetData()->AttackPower = Emotion->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("Buff ended atk returns to " + FString::SanitizeFloat(Emotion->GetData()->AttackPower));
			BuffStatusCounter = 0;
			PerkData.bBuffAtk = false;
			bIsOwnerAlreadyBuffed = false;
			bCanBuff = true;
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(GetOwner());
			UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
			Player->GetStats()->DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("Buff ended atk returns to " + FString::SanitizeFloat(Player->GetStats()->DefencePower));
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(GetOwner());
			Emotion->GetData()->DefencePower = Emotion->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("Buff ended atk returns to " + FString::SanitizeFloat(Emotion->GetData()->DefencePower));
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
			PerkData.bBuffDef = false;
			bCanBuff = true;
		}
		break;
	case LowHealth:
		BuffStatusCounter = 0;
		bIsOwnerAlreadyBuffed = false;
		PerkData.bLowHealth = false;
		bCanBuff = true;
		break;

	case Shield:
		
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			BuffStatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			PerkData.bShieldDebuff = false;
		}
		break;
	
	case NoBuff:
		BuffStatusCounter = 0;
		bIsOwnerAlreadyBuffed = false;
		break;
	default:
		break;
	}
}

static int32 FreezedUpCounter = 0;

void UStatusTracker::UnfreezeChance()
{
	const float AleatoryChance = FMath::FRand();
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	DebugHelper::LogWarning("Attempting to auto freeze");
	DebugHelper::AddMessageToLog("Attempting to auto freeze");

	FreezedUpCounter++;

	switch (FreezedUpCounter)
	{
	case 1:
		{
			if (constexpr float ChanceToFreeze = 0.25f; AleatoryChance <= ChanceToFreeze)
			{
				Target->Freeze(false);
				bIsOwnerAfflicted = false;
				CurrentActiveStatus = None;
				StatusCounter = 0;
				DebugHelper::AddMessageToLog("Free from FreezedUp at 25%");
			}
		}

	case 2:
		{
			if (constexpr float ChanceToFreeze = 0.5f; AleatoryChance <= ChanceToFreeze)
			{
				Target->Freeze(false);
				bIsOwnerAfflicted = false;
				CurrentActiveStatus = None;
				StatusCounter = 0;
				DebugHelper::AddMessageToLog("Free from FreezedUp at 50%");
			}
		}
	case 3:
		{
			if (constexpr float ChanceToFreeze = 0.75f; AleatoryChance <= ChanceToFreeze)
			{
				Target->Freeze(false);
				bIsOwnerAfflicted = false;
				CurrentActiveStatus = None;
				StatusCounter = 0;
				DebugHelper::AddMessageToLog("Free from FreezedUp at 75%");
			}
		}

	case 4:
		{
			Target->Freeze(false);
			bIsOwnerAfflicted = false;
			CurrentActiveStatus = None;
			DebugHelper::AddMessageToLog("Free from FreezedUp at 100% (malus ends)");
			StatusCounter = 0;
		}
		
	}
	

	// if (constexpr float ChanceToFreeze = 0.25f; AleatoryChance <= ChanceToFreeze)
	// {
	// 	Target->Freeze(false);
	// 	bIsOwnerAfflicted = false;
	// 	CurrentActiveStatus = None;
	// 	StatusCounter = 0;
	// }
}

/*
 * First turn 0.25
 * Second turn 0.50
 * Third 0.75
 * Last : 1.0
 */

FString UStatusTracker::GetStatusName(const EAfflictedStatus& Status) const
{
	switch (Status)
	{
	case Freezed:
		return "Freezed";
	case Burn:
		return "Envy Burned";
	case EAShame:
		return "Ashamed";
	case ShieldDebuff:
		return "DebuffShield";
	case DebuffAtk:
		return "Debuff Atk";
	case DebuffDef:
		return "Debuff Def";
	case None:
		return "None";
	default:
		return "";
	}
}

FString UStatusTracker::GetBuffName(const EBuffStatus& Buff) const
{
	switch (Buff)
	{
	case AtkBuff:
		return "Atk Buff";
	case DefBuff:
		return "Def Buff";
	case LowHealth:
		return "Low Health";
	case NoBuff:
		return "No Buff";
	default:
		return "";
	}
}

void UStatusTracker::BuffFlow(const EBuffStatus& NewBuffStatus)
{
	if (!bIsOwnerAlreadyBuffed)
	{
		return;
	}

	Priority.SetNextPriorityFromBuff(NewBuffStatus);

	if (PerkData.HasBuffHightPriority(Cast<AMob>(GetOwner())))
	{
		DebugHelper::AddMessageToLog("Hight priority detected ");
		PerkData.AssignPriority(Cast<AMob>(GetOwner()));
	}
	
	BuffWith(NewBuffStatus);
	DebugHelper::LogMessage(7, FColor::Orange, "Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus));
	DebugHelper::AddMessageToLog("Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus));
	BuffStatusCounter = 0;
	CurrentBuffedStatus = NewBuffStatus;
}

void UStatusTracker::MalusFlow()
{
	if (!bIsOwnerAfflicted || CurrentActiveStatus == EAfflictedStatus::None)
	{
		return;
	}

	DebugHelper::LogMessage(9, FColor::Green, "Malus flow called");
	
	//RevertInflictedMalus(CurrentActiveStatus);
}

FInternalPerkData& UStatusTracker::GetPerkData()
{
	return PerkData;
}

FStatusPriority& UStatusTracker::GetStatusPriority()
{
	return Priority;
}

void UStatusTracker::BuffAttack()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->AttackPower += FMath::FloorToInt(
			Player->GetStats()->AttackPower * Player->GetBattleData()->BuffAtkIncrement);
		DebugHelper::LogWarning("Attack buffed " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
		DebugHelper::AddMessageToLog("Attack buffed " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetTactics()->MovePower = FMath::FloorToInt(
			Mob->GetTactics()->MovePower * Mob->GetBattleData()->EmotionAtkBuffIncrement);
		PerkData.bBuffAtk = true;
	}
}

void UStatusTracker::BuffDefence()
{
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->DefencePower += FMath::FloorToInt(
			Player->GetStats()->DefencePower * Player->GetBattleData()->BuffDefIncrement);
		DebugHelper::LogWarning("Defence buffed " + FString::SanitizeFloat(Player->GetStats()->DefencePower));
		DebugHelper::AddMessageToLog("Defence buffed " + FString::SanitizeFloat(Player->GetStats()->DefencePower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower = FMath::FloorToInt(
			Mob->GetData()->DefencePower * Mob->GetBattleData()->EmotionDefBuffIncrement);
		PerkData.bBuffDef = true;
	}
}

void UStatusTracker::BuffShield()
{
	/*
	* per il player è un counter che parte da 3 e che diminuisce per ogni debuff ricevuto (proteggendoti dal debuff)
	* per le emozioni sempre counter da 3 che diminuisce ogni loro turno
	 */
	
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		bCanDebuff = false;
		bShieldBuffed = true;
		PlayerShieldAccumulator = 3;
		DebugHelper::AddMessageToLog(Target->GetActorLabel() + " uses shield debuff. Shield left " + FString::FromInt(PlayerShieldAccumulator));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		bCanDebuff  = false;
		PerkData.bShieldDebuff = true;
		DebugHelper::AddMessageToLog(Target->GetActorLabel() + " has shield");
	}
	
}

void UStatusTracker::Heal()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		const AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetBattleHUD()->RestoreHealth();
		bIsOwnerAlreadyBuffed = false;
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetHealthBar()->Restore(Mob->GetData()->RestorePower); // maybe add a RestorePower?
		bIsOwnerAlreadyBuffed = false;
		DebugHelper::AddMessageToLog(Mob->GetActorLabel() +  " restored " + FString::SanitizeFloat(Mob->GetData()->RestorePower));
	}
}

void UStatusTracker::DebuffAtkF()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		const AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->AttackPower -= Player->GetStats()->AttackPower * Player->GetBattleData()->DebuffAtkMalus;
		DebugHelper::AddMessageToLog("Player atk value (debuff) " + FString::SanitizeFloat(Player->GetStats()->AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->AttackPower -= Mob->GetData()->AttackPower * Mob->GetBattleData()->EmotionAtkDebuffMalus;
		DebugHelper::AddMessageToLog("AI atk value (debuff) " + FString::SanitizeFloat(Mob->GetData()->AttackPower));
		PerkData.bDebuffAtk = true;
	}
}

void UStatusTracker::DebuffDefF()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		const AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Player->GetStats()->DefencePower -= Player->GetStats()->DefencePower * Player->GetBattleData()->DebuffDefMalus;
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower -= Mob->GetData()->DefencePower * Mob->GetBattleData()->EmotionDefDebuffMalus;
		PerkData.bDebuffDef = true;
	}
}

void UStatusTracker::RevertInflictedMalus(const EAfflictedStatus& Status)
{
	if (!bIsOwnerAfflicted)
	{
		return;
	}

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	PerkData.Clear();

	switch (Status)
	{
	case Freezed:
		PerkData.bFreezedUp = false;
		break;
	case Burn:
		PerkData.bEnvyBurned = false;
		break;
	case EAShame:
		PerkData.bAshamed = false;
		break;
	case ShieldDebuff:
		PerkData.bShieldDebuff = false;
		break;
	case DebuffAtk:
		PerkData.bDebuffAtk = false;
		break;
	case DebuffDef:
		PerkData.bDebuffDef = false;
		break;
	default:
	case None:
		break;
	}
}

void UStatusTracker::RevertBuff()
{
	if (!bIsOwnerAlreadyBuffed)
	{
		return;
	}

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());
	PerkData.Clear();
	
	switch (CurrentBuffedStatus)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(Target);
			checkf(Player, TEXT("Player invalid at revert inflicted malus atkbuff"))
			Player->GetStats()->AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("Player Debuff atk reverted into " + FString::FromInt(Instance->GetPersistentData()->InitialAttackPower));
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(Target);
			checkf(Emotion, TEXT("Emotion invalid at revert inflicted malus atkbuff"))
			Emotion->GetData()->AttackPower = Emotion->GetAIMemory().InitialAttackPower;
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(Target);
			checkf(Player, TEXT("Player invalid at revert inflicted malus defbuff"))
			Player->GetStats()->DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("Player Debuff def reverted into " + FString::FromInt(Instance->GetPersistentData()->InitialDefencePower));
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			AMob* Emotion = Cast<AMob>(Target);
			checkf(Emotion, TEXT("Emotion invalid at revert inflicted malus defbuff"))
			Emotion->GetData()->DefencePower = Emotion->GetAIMemory().InitialDefencePower;
		}
		break;
	case LowHealth:
		break;
	case NoBuff:
		break;
	default:
		break;
	}
	
	BuffStatusCounter = 0;
	bIsOwnerAlreadyBuffed = false;
	CurrentBuffedStatus = EBuffStatus::NoBuff;
	DebugHelper::LogMessage(8, FColor::Orange, "Current buff " + GetBuffName(CurrentBuffedStatus) + " is now removed");
}

void UStatusTracker::InflictFreeze(AICC_Actor* Target)
{
	bIsOwnerAfflicted = true;
	Target->Freeze(true);
	DebugHelper::AddMessageToLog(Target->GetActorLabel() + " freezed");
	PerkData.bFreezedUp = true;
}

void UStatusTracker::InflictBurn(AICC_Actor* Target)
{
	// no buff for the next 3 actions for both AI and player
	bIsOwnerAfflicted = true;
	Target->Burn(true);
	bCanBuff = false;
	DebugHelper::LogWarning(Target->GetActorLabel() + " in envy burned state\nCan buff " + FString::FromInt(bCanBuff));
	DebugHelper::AddMessageToLog(Target->GetActorLabel() + " in envy burned state\nCan buff " + FString::FromInt(bCanBuff));
	PerkData.bEnvyBurned = true;
}

void UStatusTracker::InflictShieldDebuff(AICC_Actor* Target)
{
	// For 3 debuffs received by the enemy (player) /  Enemies turns (enemy), the target cannot be de-buffed
	bIsOwnerAfflicted = true;
	Target->ShieldDebuff(true);
	bCanDebuff = false;
	DebugHelper::AddMessageToLog("Shield debuff  inflicted to  " + Target->GetActorLabel());
	PerkData.bShieldDebuff = true;
}

void UStatusTracker::InflictAShamed(AICC_Actor* Target)
{
	// AI can't target for attack
	bIsOwnerAfflicted = true;
	Target->Ashamed(true);
	DebugHelper::LogMessage(5, FColor::FromHex("FE7743"), Target->GetActorLabel() + " can't perform attack");
	DebugHelper::AddMessageToLog(Target->GetActorLabel() + " can't perform attack");
	PerkData.bAshamed = true;
}
