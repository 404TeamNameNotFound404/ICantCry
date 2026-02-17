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
	DebugHelper::AddMessageToLog("[Status Tracker]: Set next priority from buff for Current Emotion turn");
	switch (BuffStatus)
	{
	case AtkBuff:
		SetNextBuffProcessPriority(1);
		NextBuffSource = EPrioritySource::Source_BuffAtk;
		break;
	case DefBuff:
		SetNextBuffProcessPriority(2);
		NextBuffSource = EPrioritySource::Source_BuffDef;
		break;
	// case Shield:
	// 	SetNextBuffProcessPriority(4);
	// 	NextBuffSource = EPrioritySource::Source_Shield;
	// 	break;
	case LowHealth:
		SetNextBuffProcessPriority(3);
		NextBuffSource = EPrioritySource::Source_LowHealth;
		break;
	default:
	case NoBuff:
		NextBuffSource = EPrioritySource::Source_None;
		break;
	}
}

void FStatusPriority::SetNextPriorityFromDebuff(const EDebuffStatus& Status)
{
	DebugHelper::AddMessageToLog("[Status Tracker]: Set next priority from debuff for Current Emotion turn");
	switch (Status)
	{
	// case Freezed:
	// 	NextBuffSource = EPrioritySource::Source_FreezedUp;
	// 	break;
	// case Burn:
	// 	SetNextPriority(4);
	// 	NextBuffSource = EPrioritySource::Source_EnvyBurned;
	// 	break;
	// case EAShame:
	// 	SetNextPriority(4);
	// 	NextBuffSource = EPrioritySource::Source_Ashamed;
	// 	break;
	// case ShieldDebuff:
	// 	SetNextPriority(4);
	// 	NextBuffSource = EPrioritySource::Source_Shield;
	// 	break;
	case DebuffAtk:
		SetNextPriority(1);
		NextBuffSource = EPrioritySource::Source_DebuffAtk;
		break;
	case DebuffDef:
		SetNextPriority(2);
		NextBuffSource = EPrioritySource::Source_DebuffDef;
		break;
	// case CriticHealth:
	// 	SetNextPriority(3);
	// 	NextBuffSource = EPrioritySource::Source_LowHealth;
	// 	break;
	default:
	case NoDebuff:
		SetNextPriority(0);
		NextBuffSource = EPrioritySource::Source_None;
		break;
	}
}

bool FStatusPriority::CanUsePriority(const EMobType& EmotionType, const EPrioritySource& SourcePriority) const
{
	switch (SourcePriority)
	{
	default:
	case Source_None:
		return true;
	case Source_BuffAtk:
		return EmotionType == EMobType::MobAnger || EmotionType == EMobType::MobDisgust || EmotionType == EMobType::MobFear ||
			EmotionType == EMobType::MobJealousy || EmotionType == EMobType::MobJoy || EmotionType == EMobType::MobSadness;
	case Source_DebuffAtk:
		return EmotionType == EMobType::MobAnger || EmotionType == EMobType::MobDisgust || EmotionType == EMobType::MobJealousy;
	case Source_BuffDef:
		return EmotionType == EMobType::MobCalm || EmotionType == EMobType::MobFear || EmotionType == EMobType::MobJoy;
	case Source_DebuffDef:
		return EmotionType == EMobType::MobCalm || EmotionType == EMobType::MobFear;
	case Source_LowHealth:
		return  EmotionType == EMobType::MobAnger || EmotionType == EMobType::MobAnxiety || EmotionType == EMobType::MobCalm ||
			    EmotionType == EMobType::MobDisgust || EmotionType == EMobType::MobFear || EmotionType == EMobType::MobJealousy ||
				EmotionType == EMobType::MobJoy || EmotionType == EMobType::MobSadness;
	case Source_EnvyBurned:
		return EmotionType == EMobType::MobAnger || EmotionType == EMobType::MobCalm || EmotionType == EMobType::MobFear;
	case Source_FreezedUp:
		return false;
	case Source_Ashamed:
		return EmotionType == EMobType::MobAnger || EmotionType == EMobType::MobDisgust || EmotionType == EMobType::MobFear ||
				EmotionType == EMobType::MobJealousy || EmotionType == EMobType::MobJoy || EmotionType == EMobType::MobSadness;
	case Source_Shield:
		return EmotionType == EMobType::MobCalm;
	}
}

void FStatusPriority::ClearNextBuff()
{
	NextBuffPriority = 0;
	NextBuffSource = EPrioritySource::Source_None;
}


EPrioritySource FStatusPriority::GetNextPrioritySource() const
{
	return NextBuffSource;
}

EPrioritySource FStatusPriority::GetCurrentPrioritySource() const
{
	return CurrentBuffSource;
}

void FStatusPriority::CommitNextBuff()
{
	CurrentBuffPriority = NextBuffPriority;
	CurrentBuffSource   = NextBuffSource;

	NextBuffPriority = 0;
	NextBuffSource   = EPrioritySource::Source_None;
}

bool FInternalPerkData::HasBuffHighPriority(AMob* Emotion) const
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

	Data.Clear();
	
	switch (const FStatusPriority& StatusPriority = Emotion->GetStatusTracker()->GetStatusPriority(); StatusPriority.GetNextBuffPriority())
	{
	default:
	case 0:
		{
			Data.bIdle = true;
			break;
		}
	
	case 1:
		{
			if (Emotion->GetIsIsBuffedAtk())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				
				switch (Emotion->GetMobType())
				{
				case MobAnger:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Anger switched to buff atk DT");
					break;
				case MobShame:
					break;
				case MobJoy:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: joy switched to buff atk DT");
					break;
				case MobDisgust:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: disgust switched to buff atk DT");
					break;
				case MobFear:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: fear switched to buff atk DT");
					break;
				case MobJealousy:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: jealousy switched to buff atk DT");
					break;
				case MobSadness:
					Data.bBuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: sadness switched to buff atk DT");
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					break;
				default:
					break;
				}
				
				break;
			}
	
			if (Emotion->GetPlayerDebuffAttack())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				Data.Clear();
				
				switch (Emotion->GetMobType())
				{
				case MobAnger:
					Data.bDebuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Anger switched to debuff atk DT");
					break;
				case MobShame:
					break;
				case MobJoy:
					break;
				case MobDisgust:
					Data.bDebuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Disgust switched to debuff atk DT");
					break;
				case MobFear:
					break;
				case MobJealousy:
					Data.bDebuffAtk = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Jealousy switched to debuff atk DT");
					break;
				case MobSadness:
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					break;
				default:
					break;
				}
				break;
			}
	
			break;
		}
	
	case 2:
		{
			if (Emotion->GetIsBuffedDefence())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
				

				switch (Emotion->GetMobType())
				{
				case MobAnger:
					break;
				case MobShame:
					break;
				case MobJoy:
					Data.bBuffDef = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Joy switched to buffdef DT");
					break;
				case MobDisgust:
					break;
				case MobFear:
					DebugHelper::AddMessageToLog("[Status Tracker]: Fear switched to buffdef DT");
					Data.bBuffDef = true;
					break;
				case MobJealousy:
					break;
				case MobSadness:
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					Data.bBuffDef = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: Calm switched to buffdef DT");
					break;
				default:
					break;
				}
				
				break;
			}
	
			if (Emotion->GetIsTargetDefenceDebuffed())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));

				switch (Emotion->GetMobType())
				{
				case MobAnger:
					break;
				case MobShame:
					break;
				case MobJoy:
					break;
				case MobDisgust:
					break;
				case MobFear:
					Data.bDebuffDef = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: fear switched to debuff def DT");
					break;
				case MobJealousy:
					break;
				case MobSadness:
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					Data.bDebuffDef = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: calm switched to debuff def DT");
					break;
				default:
					break;
				}
				
				break;
			}
	
			break;
		}
	
	case 3:
		{
			DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT priority " + FString::FromInt(StatusPriority.GetNextBuffPriority()));
			
			
			switch (Emotion->GetMobType())
			{
			case MobAnger:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: anger switched to low DT");
				break;
			case MobShame:
				break;
			case MobJoy:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: joy switched to low DT");
				break;
			case MobDisgust:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: disgust switched to low DT");
				break;
			case MobFear:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: fear switched to low DT");
				break;
			case MobJealousy:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: jealocoso switched to low DT");
				break;
			case MobSadness:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: sadcoso switched to low DT");
				break;
			case MobAnxiety:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: anxicoso switched to low DT");
				break;
			case MobCalm:
				Data.bLowHealth = true;
				DebugHelper::AddMessageToLog("[Status Tracker]: calm switched to low DT");
				break;
			default:
				break;
			}
			
			break;
		}
	
	case 4:
		{
			if (Emotion->GetIsIsEnvyBurned())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: " + Emotion->GetActorLabel() + " changed it's DT to Envy Burned");
				

				switch (Emotion->GetMobType())
				{
				case MobAnger:
					Data.bEnvyBurned = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: anger switched to burn DT");
					break;
				case MobShame:
					break;
				case MobJoy:
					break;
				case MobDisgust:
					break;
				case MobFear:
					Data.bEnvyBurned = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: fear switched to burn DT");
					break;
				case MobJealousy:
					break;
				case MobSadness:
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					Data.bEnvyBurned = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: calm switched to burn DT");
					break;
				default:
					break;
				}
				
				break;
			}
	
			if (Emotion->GetIsDebuffShield())
			{

				switch (Emotion->GetMobType())
				{
				case MobAnger:
					break;
				case MobShame:
					break;
				case MobJoy:
					break;
				case MobDisgust:
					break;
				case MobFear:
					break;
				case MobJealousy:
					break;
				case MobSadness:
					break;
				case MobAnxiety:
					break;
				case MobCalm:
					Data.bShieldDebuff = true;
					DebugHelper::AddMessageToLog("[Status Tracker]: calm switched to shield DT");
					break;
				default:
					break;
				}
				break;
			}

			break;
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

	Instance = Cast<UICantCryGameInstance>(GetWorld()->GetGameInstance());

	BuffCounters.Add(EBuffStatus::AtkBuff, 0);
	BuffCounters.Add(EBuffStatus::DefBuff, 0);
	//BuffCounters.Add(EBuffStatus::Shield, 0);
}


bool UStatusTracker::IsAfflicted() const
{
	return bIsOwnerAfflicted;
}

bool UStatusTracker::IsBuffed() const
{
	return bIsOwnerAlreadyBuffed;
}

bool UStatusTracker::CanDebuff() const
{
	return bCanDebuff;
}


void UStatusTracker::InflictStatus(const EAfflictedStatus& Status, AICC_Actor* Target)
{
	if (Target->GetStatusTracker()->IsAfflicted() || !Target->GetStatusTracker()->CanDebuff()) // bIsOwnerAfflicted || !bCanDebuff
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: Emotion attempted to cast " + GetStatusName(Status) + " To " + Target->GetActorLabel() + " but " + GetStatusName(CurrentActiveStatus) + " has already been inflicted");
		return;
	}

	if (bShieldBuffed)
	{
		PlayerShieldAccumulator--;
		DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " attempted to inflict " + GetStatusName(Status) + " but fortunately a shield was used to protect from the debuff "
			+ FString::FromInt(PlayerShieldAccumulator) + " left");

		if (PlayerShieldAccumulator <= 0)
		{
			bShieldBuffed = false;
			PlayerShieldAccumulator = 0;
			DebugHelper::AddMessageToLog("[Status Tracker]: Shield debuff protection for " + GetOwner()->GetActorLabel() + " ended");
		}
		else
		{
			return;
		}
	}

	//Priority.SetNextPriorityFromDebuff(Status);

	// if (GetOwner()->IsA(AMob::StaticClass()))
	// {
	// 	AMob* Emotion = Cast<AMob>(GetOwner());
	// 	
	// 	if (PerkData.HasHighDebuffPriority(Cast<AMob>(GetOwner())))
	// 	{
	// 		PerkData.AssignPriority(Emotion);
	// 		StatusCounter = 0;
	// 		bIsOwnerAfflicted = true;
	// 	}
	// }

	CurrentActiveStatus = Status;
	StatusCounter = 0;
	bIsOwnerAfflicted = true;

	DebugHelper::LogMessage(6, FColor::Black, "Inflicting  " + GetStatusName(CurrentActiveStatus) + " To " + Target->GetActorLabel());
	DebugHelper::AddMessageToLog("[Status Tracker]: Inflicting  " + GetStatusName(CurrentActiveStatus) + " To " + Target->GetActorLabel());

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
		BuffShield();
		break;
	// case DebuffAtk:
	// 	Priority.SetBuffCurrentPriority(1);
	// 	DebuffAtkF(Target);
	// 	break;
	// case DebuffDef:
	// 	Priority.SetBuffCurrentPriority(2);
	// 	DebuffDefF(Target);
	// 	break;
	case CriticHealth:
		Priority.SetBuffCurrentPriority(3);
		if (GetOwner()->IsA(AMob::StaticClass()))
		{
			if (const AMob* Shame = Cast<AMob>(GetOwner()); Shame->IsEShame())
			{
				DebugHelper::AddMessageToLog("[Status Tracker]: Mob is shame, can't go low health, skipping low health decision table");
				return;
			}
			
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " is in low health status!");
			PerkData.bLowHealth = true;
		}
	default:
	case None:
		break;
	}
}

void UStatusTracker::InflictDebuffStatus(const EDebuffStatus& Status, AICC_Actor* Target)
{
	if (Target->GetStatusTracker()->IsAfflicted() || !Target->GetStatusTracker()->CanDebuff())
	{
		DebugHelper::AddMessageToLog("[Status Tracker - DeBuffS]: Emotion attempted to cast " + GetDebuffName(CurrentDebuffStatus) + " To " + Target->GetActorLabel() + " but " + GetStatusName(CurrentActiveStatus) + " has already been inflicted");
		return;
	}

	if (bShieldBuffed)
	{
		PlayerShieldAccumulator--;
		DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " attempted to inflict " + GetDebuffName(CurrentDebuffStatus) + " but fortunately a shield was used to protect from the debuff "
			+ FString::FromInt(PlayerShieldAccumulator) + " left");

		if (PlayerShieldAccumulator <= 0)
		{
			bShieldBuffed = false;
			PlayerShieldAccumulator = 0;
			DebugHelper::AddMessageToLog("[Status Tracker]: Shield debuff protection for " + GetOwner()->GetActorLabel() + " ended");
		}
		else
		{
			return;
		}
	}

	Priority.SetNextPriorityFromDebuff(Status);

	if (GetOwner()->IsA(AMob::StaticClass()))
	{
		AMob* Emotion = Cast<AMob>(GetOwner());
		
		if (PerkData.HasHighDebuffPriority(Cast<AMob>(GetOwner())))
		{
			PerkData.AssignPriority(Emotion);
			StatusCounter = 0;
			bIsOwnerAfflicted = true;
		}
	}


	CurrentDebuffStatus = Status;
	DebuffCounter = 0;
	bIsOwnerDebuffed = true;
	
	DebugHelper::LogMessage(6, FColor::Black, "Inflicting  " + GetDebuffName(CurrentDebuffStatus) + " To " + Target->GetActorLabel());
	DebugHelper::AddMessageToLog("[Status Tracker]: Inflicting  " + GetDebuffName(CurrentDebuffStatus) + " To " + Target->GetActorLabel());

	switch (Status)
	{
	case DebuffAtk:
		Priority.SetBuffCurrentPriority(1);
		DebuffAtkF(Target);
		break;
	case DebuffDef:
		Priority.SetBuffCurrentPriority(2);
		DebuffDefF(Target);
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

	if (static_cast<int32>(CurrentBuffedStatus) == static_cast<int32>(BuffStatus))
	{
		bBuffedTwice = true;
		DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " got buffed twice with " + GetBuffName(BuffStatus));
		DebugHelper::AddMessageToLog("[Status Tracker]: Current status of " + GetOwner()->GetActorLabel() + " is still " + GetBuffName(CurrentBuffedStatus));
		BuffStatusCounter = 0;
		return;
	}

	CurrentBuffedStatus = BuffStatus;
	BuffStatusCounter = 0;

	CurrentBuffedStatus = BuffStatus;
	BuffCounters.FindOrAdd(BuffStatus) = 0;

	bIsOwnerAlreadyBuffed = true;

	switch (BuffStatus)
	{
	case AtkBuff:
		Priority.SetPriotity(1);
		
		if (!bBuffedTwice)
		{
			BuffAttack();
		}
		
		break;
	case DefBuff:
		Priority.SetPriotity(2);

		if (!bBuffedTwice)
		{
			BuffDefence();
		}
		
		//BuffDefence();
		break;
	case LowHealth:
		Heal();
		break;
	// case Shield:
	// 	Priority.SetPriotity(4);
	// 	BuffShield();
	// 	break;
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
			DebugHelper::AddMessageToLog("[Status Tracker]: " + Self->GetActorLabel() + " health is not low");
			PerkData.bLowHealth = false;
		}
	}
	
	if (!bIsOwnerAfflicted || !bCanDebuff)
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: No Status for " + GetOwner()->GetActorLabel());
		return;
	}

	StatusCounter += 1;

	DebugHelper::LogWarning("Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	DebugHelper::AddMessageToLog("[Status Tracker]: Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	

	if (StatusCounter < 3)
	{
		return;
	}

	RevertInflictedMalus(CurrentActiveStatus);

	DebugHelper::AddMessageToLog("[Status Tracker]: " + GetStatusName(CurrentActiveStatus) + " has been reset for " + GetOwner()->GetActorLabel());
	
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
		
		CurrentActiveStatus = EAfflictedStatus::None;
		
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
		CurrentActiveStatus = EAfflictedStatus::None;
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
		CurrentActiveStatus = EAfflictedStatus::None;
		break;
	case ShieldDebuff:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Target->ShieldDebuff(false);
		bCanDebuff = true;
		
		if (GetOwner()->IsA(AICC_Player::StaticClass()))
		{
			PerkData.bShieldDebuff = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
			DebugHelper::AddMessageToLog("[Status Tracker]: Shield Debuff for " + GetOwner()->GetActorLabel() + " has ended");
		}

		
		if (Target->IsA(AMob::StaticClass()))
		{
			StatusCounter = 0;
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			PerkData.bShieldDebuff = false;
			bBuffedTwice = false;
		}

		CurrentActiveStatus = EAfflictedStatus::None;
		
		break;
	default:
	case None:
		bIsOwnerAfflicted = false;
		StatusCounter = 0;
		Priority.SetPriotity(0);
		CurrentActiveStatus = EAfflictedStatus::None;
		break;
	}
}

void UStatusTracker::UpdateDebuffStatus()
{
	
	if (!bIsOwnerDebuffed)
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: No debuff status found for " + GetOwner()->GetActorLabel());
		return;
	}

	DebuffCounter += 1;

	DebugHelper::LogWarning("Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	DebugHelper::AddMessageToLog("[Status Tracker]: Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	

	if (DebuffCounter < 3)
	{
		return;
	}

	RevertInflictedMalus(CurrentActiveStatus);

	DebugHelper::AddMessageToLog("[Status Tracker]: " + GetDebuffName(CurrentDebuffStatus) + " has been reset for " + GetOwner()->GetActorLabel());

	switch (CurrentDebuffStatus)
	{
	case DebuffAtk:
		bIsOwnerDebuffed = false;
		DebuffCounter = 0;

		if (GetOwner()->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(GetOwner());
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Debuff for Atk ended, atk of " + Player->GetActorLabel() + " restored as " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		}
		
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bDebuffAtk = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		
		CurrentDebuffStatus = EDebuffStatus::NoDebuff;
		break;
	case DebuffDef:
		bIsOwnerDebuffed = false;
		DebuffCounter = 0;

		if (GetOwner()->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(GetOwner());
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Debuff for Def ended, def of " + Player->GetActorLabel() + " restored as " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
		}
		
		if (Cast<AMob>(GetOwner()))
		{
			PerkData.bDebuffDef = false;
			bIsOwnerAfflicted = false;
			StatusCounter = 0;
			Priority.SetPriotity(0);
		}
		
		CurrentDebuffStatus = EDebuffStatus::NoDebuff;
		break;
		default:
		break;
	}
}

void UStatusTracker::UpdateBuffStatus()
{
	if (!bIsOwnerAlreadyBuffed || !bCanBuff)
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: No buff status found for " + GetOwner()->GetActorLabel());
		return;
	}

	BuffStatusCounter += 1;
	int32& Counter = BuffCounters.FindOrAdd(CurrentBuffedStatus);
	Counter++;

	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	for (const auto& Pair : BuffCounters)
	{
		if (Pair.Key == EBuffStatus::NoBuff)
		{
			continue;
		}

		if (Pair.Value > 0)
		{
			DebugHelper::AddMessageToLog( "[Status Tracker]: " + Target->GetActorLabel() +
				" Buff Status counter of " + GetBuffName(Pair.Key) + " " + FString::FromInt(Pair.Value)
			);
		}
	}

	
	if (Counter < 3)
	{
		return; // I don't need to check further if counter is not 3 (3 turns elapsed)
	}

	switch (CurrentBuffedStatus)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			DebugHelper::AddMessageToLog("[Status Tracker]: Player buff before returning back to normal " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Player Buff ended atk returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			Counter = 0;
			
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->AttackPower));
			Emotion->GetData()->AttackPower = Emotion->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, atk returns to " + FString::SanitizeFloat(Emotion->GetData()->AttackPower));
			Counter = 0;
			PerkData.bBuffAtk = false;
			bIsOwnerAlreadyBuffed = false;
			bCanBuff = true;
			bBuffedTwice = false;
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff ended def returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
			Counter = 0;
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->DefencePower));
			Emotion->GetData()->DefencePower = Emotion->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, def returns to " + FString::SanitizeFloat(Emotion->GetData()->DefencePower));
			Counter = 0;
			bIsOwnerAlreadyBuffed = false;
			PerkData.bBuffDef = false;
			bCanBuff = true;
			bBuffedTwice = false;
		}
		break;
	case LowHealth:
		Counter = 0;
		bIsOwnerAlreadyBuffed = false;
		PerkData.bLowHealth = false;
		bCanBuff = true;
		bBuffedTwice = false;
		break;

	// case Shield:
	// 	
	// 	if (Target->IsA(AICC_Player::StaticClass()))
	// 	{
	// 		Counter = 0;
	// 		bIsOwnerAlreadyBuffed = false;
	// 	}
	// 	if (Target->IsA(AMob::StaticClass()))
	// 	{
	// 		Counter = 0;
	// 		bIsOwnerAlreadyBuffed = false;
	// 		bCanDebuff = true;
	// 		PerkData.bShieldDebuff = false;
	// 		bBuffedTwice = false;
	// 	}
	// 	break;
	
	case NoBuff:
		Counter = 0;
		bIsOwnerAlreadyBuffed = false;
		bBuffedTwice = false;
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
	DebugHelper::AddMessageToLog("[Status Tracker]: Attempting to auto freeze");

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
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 25%");
			}

			break;
		}

	case 2:
		{
			if (constexpr float ChanceToFreeze = 0.5f; AleatoryChance <= ChanceToFreeze)
			{
				Target->Freeze(false);
				bIsOwnerAfflicted = false;
				CurrentActiveStatus = None;
				StatusCounter = 0;
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 50%");
			}
			
			break;
		}
	case 3:
		{
			if (constexpr float ChanceToFreeze = 0.75f; AleatoryChance <= ChanceToFreeze)
			{
				Target->Freeze(false);
				bIsOwnerAfflicted = false;
				CurrentActiveStatus = None;
				StatusCounter = 0;
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 75%");
			}
			
			break;
		}

	case 4:
		{
			Target->Freeze(false);
			bIsOwnerAfflicted = false;
			CurrentActiveStatus = None;
			DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 100% (malus ends)");
			StatusCounter = 0;
			FreezedUpCounter = 0;
			
			break;
		}
	
	default:
		FreezedUpCounter = 0;
		break;
		
	}
}

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
	case None:
		return "None";
	default:
		return "";
	}
}

FString UStatusTracker::GetDebuffName(const EDebuffStatus& Status) const
{
	switch (Status)
	{
	case DebuffAtk:
		return "Debuff Atk";
	case DebuffDef:
		return "Debuff Def";
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
	default:
	case NoBuff:
		return "No Buff";
	}
}


/*
* Buff Atk riceve buff atk -> counter a 0 ma buff comunque attivo
* Buff Atk con Debuff atk -> counter a 0, buff annullato
* Debuff Atk con Debuff Atk -> counter a 0, Debuff comunque attivo
* Debuff Atk con Buff Atk -> counter a 0, debuff annullato
 */

void UStatusTracker::BuffFlow(const EBuffStatus& NewBuffStatus)
{
	if (bIsOwnerAlreadyBuffed)
	{
		return;
	}

	const AMob* Emotion = Cast<AMob>(GetOwner());
	
	if (!Emotion)
	{
		return;
	}
	
	Priority.SetNextPriorityFromBuff(NewBuffStatus);
	
	if (const FStatusPriority& P = Priority; !P.CanUsePriority(Emotion->GetMobType(), P.GetNextPrioritySource()))
	{
		DebugHelper::AddMessageToLog("[Status Tracker - Buff flow]: " + Emotion->GetActorLabel() +
			" cannot use priority source " + UEnum::GetValueAsString(P.GetNextPrioritySource()));

		Priority.ClearNextBuff();
		return;
	}

	switch (CurrentBuffedStatus)
	{
	case EBuffStatus::AtkBuff:
		Emotion->GetData()->AttackPower = Emotion->GetAIMemory().InitialAttackPower;
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: " + Emotion->GetActorLabel() + " reverted it's atk power into " + FString::FromInt(Emotion->GetData()->AttackPower));
		break;
	case EBuffStatus::DefBuff:
		Emotion->GetData()->DefencePower = Emotion->GetAIMemory().InitialDefencePower;
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: " + Emotion->GetActorLabel() + " reverted it's def power into " + FString::FromInt(Emotion->GetData()->DefencePower));
		break;
	case EBuffStatus::LowHealth:
	case EBuffStatus::NoBuff:
	default:
		break;
	}
	
	BuffWith(NewBuffStatus);

	if (PerkData.HasBuffHighPriority(Cast<AMob>(GetOwner())))
	{
		CurrentBuffedStatus = NewBuffStatus;
		int32& Counter = BuffCounters.FindOrAdd(CurrentBuffedStatus);
		Counter = 0;
		
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: High priority detected  ... " + GetOwner()->GetActorLabel() +
			" received " + GetBuffName(NewBuffStatus) + " and counter has been reset to" + FString::FromInt(Counter));
		PerkData.AssignPriority(Cast<AMob>(GetOwner()));
	}
	
	CurrentBuffedStatus = NewBuffStatus;
	int32& Counter = BuffCounters.FindOrAdd(CurrentBuffedStatus);
	
	Counter = 0;
	// CurrentBuffedStatus = NewBuffStatus;
	DebugHelper::LogMessage(7, FColor::Orange, "Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus));
	DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus) + " and counter has been reset to "
		+ FString::FromInt(Counter));
}

void UStatusTracker::DebuffFlow(const EDebuffStatus& NewDebuffStatus, AICC_Actor* Target)
{
	if (!bIsOwnerDebuffed)
	{
		DebugHelper::AddMessageToLog("[Status Tracker - Debuff Flow]: " + GetOwner()->GetActorLabel() + " atm is not debuff affected");
		return;
	}

	if (CurrentDebuffStatus == NewDebuffStatus)
	{
		DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: " + GetOwner()->GetActorLabel() + " does not need to check collision with actual debuff status");
		DebuffCounter = 0;
		return;
	}

	if (NewDebuffStatus == EDebuffStatus::DebuffAtk && CurrentBuffedStatus == EBuffStatus::AtkBuff)
	{
		DebuffCounter = 0;

		if (Target && Target->IsA(AICC_Player::StaticClass()))
		{
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: " + Target->GetActorLabel() + " collided with " + GetDebuffName(NewDebuffStatus) + " but it has " + GetBuffName(CurrentBuffedStatus) + " so counter and stats are reverted");
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: Counter now is " + FString::FromInt(DebuffCounter) + " and the attack power now restored as " + FString::FromInt(Instance->GetRuntimeStats().AttackPower));
			return;
		}

		if (Target && Target->IsA(AMob::StaticClass()))
		{
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: " + Target->GetActorLabel() + " collided with " + GetDebuffName(NewDebuffStatus) + " but it has " + GetBuffName(CurrentBuffedStatus) + " so counter and stats are reverted");
			const AMob* E = Cast<AMob>(Target);
			E->GetData()->AttackPower = E->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: Counter now is " + FString::FromInt(DebuffCounter) + " and the attack power now restored as " + FString::FromInt(E->GetData()->AttackPower));
			return;
		}
	}


	if (NewDebuffStatus == EDebuffStatus::DebuffAtk && CurrentBuffedStatus == EBuffStatus::DefBuff)
	{
		DebuffCounter = 0;

		if (Target && Target->IsA(AICC_Player::StaticClass()))
		{
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: " + Target->GetActorLabel() + " collided with " + GetDebuffName(NewDebuffStatus) + " but it has " + GetBuffName(CurrentBuffedStatus) + " so counter and stats are reverted");
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: Counter now is " + FString::FromInt(DebuffCounter) + " and the defence power now restored as " + FString::FromInt(Instance->GetRuntimeStats().DefencePower));
			return;
		}

		if (Target && Target->IsA(AMob::StaticClass()))
		{
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: " + Target->GetActorLabel() + " collided with " + GetDebuffName(NewDebuffStatus) + " but it has " + GetBuffName(CurrentBuffedStatus) + " so counter and stats are reverted");
			const AMob* E = Cast<AMob>(Target);
			E->GetData()->DefencePower = E->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker - Debuff flow]: Counter now is " + FString::FromInt(DebuffCounter) + " and the attack power now restored as " + FString::FromInt(E->GetData()->DefencePower));
			return;
		}
	}
}

void UStatusTracker::BuffFlow(const EBuffStatus& NewBuffStatus, AMob* Target)
{
	const AMob* Emotion = Cast<AMob>(GetOwner());
	
	if (!Emotion)
	{
		return;
	}

	BuffWith(NewBuffStatus);
	
	Priority.SetNextPriorityFromBuff(NewBuffStatus);
	
	if (const FStatusPriority& P = Priority; !P.CanUsePriority(Emotion->GetMobType(), P.GetNextPrioritySource()))
	{
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: " + Emotion->GetActorLabel() +
			" cannot use priority source " + UEnum::GetValueAsString(P.GetNextPrioritySource()));

		Priority.ClearNextBuff();
	}
	else
	{
		if (PerkData.HasBuffHighPriority(Cast<AMob>(GetOwner()))) // probably i have to set to target Cast<AMob>(GetOwner())
		{
			DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: High priority detected ");
			PerkData.AssignPriority(Target);
		}
	}
	
	DebugHelper::LogMessage(7, FColor::Orange, "Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus));
	DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: Old buff " + GetBuffName(CurrentBuffedStatus) + "Removed " + "New buff assigned " + GetBuffName(NewBuffStatus));
	BuffStatusCounter = 0;
	CurrentBuffedStatus = NewBuffStatus;
}

void UStatusTracker::MalusFlow(const EAfflictedStatus& NewStatus)
{
	if (!bIsOwnerAfflicted)
	{
		return;
	}
	
	StatusCounter = 0;
	CurrentActiveStatus = NewStatus;
}

FInternalPerkData& UStatusTracker::GetPerkData()
{
	return PerkData;
}

FStatusPriority& UStatusTracker::GetStatusPriority()
{
	return Priority;
}

void UStatusTracker::Reset()
{
	bIsOwnerAfflicted = false;
	bIsOwnerAlreadyBuffed = false;
	bShieldBuffed = false;
	Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
	Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
	Instance->GetRuntimeStats().ApModifier = 1;
	DebugHelper::LogMessage(8, FColor::Blue, "Stats successfully restored");
}

void UStatusTracker::BuffAttack()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Instance->GetRuntimeStats().AttackPower += FMath::FloorToInt( Instance->GetRuntimeStats().AttackPower * Player->GetBattleData()->BuffAtkIncrement);
		DebugHelper::LogWarning("[Status Tracker]: " + Player->GetActorLabel() + " buffed it's attack " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Player->GetActorLabel() + " buffed it's attack " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		const int32 BuffedAtk = FMath::FloorToInt( Mob->GetData()->AttackPower * Mob->GetBattleData()->EmotionAtkBuffIncrement);
		Mob->GetData()->AttackPower += BuffedAtk;

		switch (Mob->GetMobType())
		{
		case MobAnger:
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk " + FString::SanitizeFloat(Mob->GetTactics()->MovePower) +
				" to " + FString::SanitizeFloat(BuffedAtk));
			PerkData.Clear();
			PerkData.bBuffAtk = true;
			break;
		case MobShame:
			PerkData.bBuffAtk = false;
			break;
		case MobJoy:
			PerkData.bBuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk");
			break;
		case MobDisgust:
			PerkData.bBuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk");
			break;
		case MobFear:
			PerkData.bBuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk");
			break;
		case MobJealousy:
			PerkData.bBuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk");
			break;
		case MobSadness:
			PerkData.bBuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff atk is applied and " + Mob->GetActorLabel() + " changed it's DT to buff atk");
			break;
		case MobAnxiety:
			PerkData.bBuffAtk = false;
			break;
		case MobCalm:
			PerkData.bBuffAtk = false;
			break;
		default:
			PerkData.bBuffAtk = false;
			break;
		}
	}
}

void UStatusTracker::BuffDefence()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		const AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		Instance->GetRuntimeStats().DefencePower += FMath::FloorToInt(Instance->GetRuntimeStats().DefencePower * Player->GetBattleData()->BuffDefIncrement);
		DebugHelper::LogWarning("[Status Tracker]: " + Player->GetActorLabel() +  "buffed it's Defence - " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Player->GetActorLabel() +  "buffed it's Defence - " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower += FMath::FloorToInt(Mob->GetData()->DefencePower * Mob->GetBattleData()->EmotionDefBuffIncrement);
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Mob->GetActorLabel() + " buffed it's def " + FString::FromInt(Mob->GetData()->DefencePower));

		switch (Mob->GetMobType())
		{
		case MobAnger:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobShame:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobJoy:
			PerkData.Clear();
			PerkData.bBuffDef = true;
			break;
		case MobDisgust:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobFear:
			PerkData.Clear();
			PerkData.bBuffDef = true;
			break;
		case MobJealousy:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobSadness:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobAnxiety:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		case MobCalm:
			PerkData.Clear();
			PerkData.bBuffDef = true;
			break;
		default:
			PerkData.Clear();
			PerkData.bBuffDef = false;
			break;
		}
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
		DebugHelper::LogWarning("[Status Tracker]: " + Target->GetActorLabel() + " uses shield debuff. Shield left " + FString::FromInt(PlayerShieldAccumulator));
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " uses shield debuff. Shield left " + FString::FromInt(PlayerShieldAccumulator));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		bCanDebuff  = false;
		// PerkData.bShieldDebuff = true;
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " has shield");
		
		switch (const AMob* Mob = Cast<AMob>(GetOwner()); Mob->GetMobType())
		{
		case MobAnger:
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			break;
		case MobFear:
			break;
		case MobJealousy:
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			PerkData.Clear();
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table for " + Target->GetActorLabel() + " has changed into Shield");
			PerkData.bShieldDebuff = true;
			break;
		}
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
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Mob->GetActorLabel() +  " restored " + FString::SanitizeFloat(Mob->GetData()->RestorePower));
	}
}

void UStatusTracker::DebuffAtkF()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math :starting Debuff atk" + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		Instance->GetRuntimeStats().AttackPower -= Instance->GetRuntimeStats().AttackPower * Player->GetBattleData()->DebuffAtkMalus;
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math : Debuff atk post malus -> " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower) +
			"\n[Status Tracker]: Atk malus dealed -> " + FString::SanitizeFloat(Player->GetBattleData()->DebuffAtkMalus));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->AttackPower -= Mob->GetData()->AttackPower * Mob->GetBattleData()->EmotionAtkDebuffMalus;
		DebugHelper::LogSuccess("[Status Tracker]: " +Mob->GetActorLabel() + "got it's atk de-buffed now has " + FString::SanitizeFloat(Mob->GetData()->AttackPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: AI atk value (debuff) " + FString::SanitizeFloat(Mob->GetData()->AttackPower));

		switch (Mob->GetMobType())
		{
		case MobAnger:
			PerkData.bDebuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			PerkData.bDebuffAtk = true;
			break;
		case MobFear:
			break;
		case MobJealousy:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			PerkData.bDebuffAtk = true;
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			break;
		}
	}
}

void UStatusTracker::DebuffAtkF(AICC_Actor* Target)
{
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math :starting Debuff atk" + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		Instance->GetRuntimeStats().AttackPower -= Instance->GetRuntimeStats().AttackPower * Player->GetBattleData()->DebuffAtkMalus;
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math : Debuff atk post malus -> " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower) +
			"\n[Status Tracker]: Atk malus dealed -> " + FString::SanitizeFloat(Player->GetBattleData()->DebuffAtkMalus));
	}
	
	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(Target);
		Mob->GetData()->AttackPower -= Mob->GetData()->AttackPower * Mob->GetBattleData()->EmotionAtkDebuffMalus;
		DebugHelper::LogSuccess("[Status Tracker]: " +Mob->GetActorLabel() + "got it's atk de-buffed now has " + FString::SanitizeFloat(Mob->GetData()->AttackPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: AI atk value (debuff) " + FString::SanitizeFloat(Mob->GetData()->AttackPower));

		switch (Mob->GetMobType())
		{
		case MobAnger:
			PerkData.bDebuffAtk = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			PerkData.bDebuffAtk = true;
			break;
		case MobFear:
			break;
		case MobJealousy:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff atk");
			PerkData.bDebuffAtk = true;
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			break;
		}
	}
}

void UStatusTracker::DebuffDefF()
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math :starting Debuff def" + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
		Instance->GetRuntimeStats().DefencePower -= Instance->GetRuntimeStats().DefencePower * Player->GetBattleData()->DebuffDefMalus;
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math : Debuff def post malus -> " +
			FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower) + "\n[Status Tracker]: Def malus dealed -> " + FString::SanitizeFloat(Player->GetBattleData()->DebuffDefMalus));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower -= Mob->GetData()->DefencePower * Mob->GetBattleData()->EmotionDefDebuffMalus;
		
		switch (Mob->GetMobType())
		{
		case MobAnger:
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			break;
		case MobFear:
			PerkData.bDebuffDef = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			break;
		case MobJealousy:
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			PerkData.bDebuffDef = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			break;
		default:
			break;
		}
	}
}

void UStatusTracker::DebuffDefF(AICC_Actor* Target)
{

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(GetOwner());
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math :starting Debuff def" + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
		Instance->GetRuntimeStats().DefencePower -= Instance->GetRuntimeStats().DefencePower * Player->GetBattleData()->DebuffDefMalus;
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math : Debuff def post malus -> " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower) +
			"\n[Status Tracker]: Def malus dealed -> " + FString::SanitizeFloat(Player->GetBattleData()->DebuffDefMalus));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(GetOwner());
		Mob->GetData()->DefencePower -= Mob->GetData()->DefencePower * Mob->GetBattleData()->EmotionDefDebuffMalus;
		
		switch (Mob->GetMobType())
		{
		case MobAnger:
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			break;
		case MobFear:
			PerkData.bDebuffDef = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			break;
		case MobJealousy:
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			PerkData.bDebuffDef = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			break;
		default:
			break;
		}
	}
}

void UStatusTracker::RevertInflictedMalus(const EAfflictedStatus& Status)
{
	if (!bIsOwnerAfflicted)
	{
		return;
	}
	
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
	default:
	case None:
		break;
	}
}

void UStatusTracker::RevertDebuff()
{
	if (!bIsOwnerDebuffed)
	{
		return;
	}

	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	PerkData.Clear();

	switch (CurrentDebuffStatus)
	{
	case DebuffAtk:
		PerkData.bDebuffAtk = false;
		break;
	case DebuffDef:
		PerkData.bDebuffDef = false;
		break;
	default:
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
	
	PerkData.Clear();
	
	switch (CurrentBuffedStatus)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Cast<AICC_Player>(Target);
			checkf(Player, TEXT("Player invalid at revert inflicted malus atkbuff"))
			Instance->GetRuntimeStats().AttackPower  = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Player Debuff atk reverted into " + FString::FromInt(Instance->GetPersistentData()->InitialAttackPower));
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
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Player Debuff def reverted into " + FString::FromInt(Instance->GetPersistentData()->InitialDefencePower));
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

void UStatusTracker::ApplyPriorityBuff(const EBuffStatus& BuffStatus, AMob* Target)
{
	Priority.SetNextPriorityFromBuff(BuffStatus);
	if (!Priority.CanUsePriority(Cast<AMob>(GetOuter())->GetMobType(), Priority.GetNextPrioritySource()))
	{
		Priority.ClearNextBuff();
		return;
	}

	Priority.SetBuffCurrentPriority(Priority.GetNextBuffPriority());
	Priority.CommitNextBuff(); 
	PerkData.AssignPriority(Cast<AMob>(GetOwner()));
}

void UStatusTracker::InflictFreeze(AICC_Actor* Target)
{
	bIsOwnerAfflicted = true;
	Target->Freeze(true);
	DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " freezed");
	PerkData.bFreezedUp = true;
}

void UStatusTracker::InflictBurn(AICC_Actor* Target)
{
	// no buff for the next 3 actions for both AI and player
	bIsOwnerAfflicted = true;
	Target->Burn(true);
	bCanBuff = false;
	DebugHelper::LogWarning(Target->GetActorLabel() + " in envy burned state\nCan buff " + FString::FromInt(bCanBuff));
	DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " in envy burned state\nCan buff " + FString::FromInt(bCanBuff));

	if (Target->IsA(AMob::StaticClass()))
	{
		switch (const AMob* Mob = Cast<AMob>(Target); Mob->GetMobType())
		{
		case MobAnger:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now envy burned");
			PerkData.bEnvyBurned = true;
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			break;
		case MobFear:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now envy burned");
			PerkData.bEnvyBurned = true;
			break;
		case MobJealousy:
			break;
		case MobSadness:
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now envy burned");
			PerkData.bEnvyBurned = true;
			break;
		default:
			break;
		}
	}
}

void UStatusTracker::InflictShieldDebuff(AICC_Actor* Target)
{
	// For 3 debuffs received by the enemy (player) /  Enemies turns (enemy), the target cannot be de-buffed
	bIsOwnerAfflicted = true;
	Target->ShieldDebuff(true);
	bCanDebuff = false;
	DebugHelper::LogWarning("[Status Tracker]: Shield debuff  inflicted to  " + Target->GetActorLabel());
	DebugHelper::AddMessageToLog("[Status Tracker]: Shield debuff  inflicted to  " + Target->GetActorLabel());
	PerkData.bShieldDebuff = true;
}

void UStatusTracker::InflictAShamed(AICC_Actor* Target)
{
	// AI can't target for attack and player can't attack
	
	bIsOwnerAfflicted = true;
	Target->Ashamed(true);
	DebugHelper::LogMessage(5, FColor::FromHex("FE7743"), Target->GetActorLabel() + " can't perform attack");
	DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " can't perform attack");

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(Target);
		Player->Ashamed(true);
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		switch (const AMob* Mob = Cast<AMob>(Target); Mob->GetMobType())
		{
		case MobAnger:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			PerkData.bAshamed = true;
			break;
		case MobShame:
			break;
		case MobJoy:
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobDisgust:
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobFear:
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobJealousy:
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobSadness:
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobAnxiety:
			break;
		case MobCalm:
			break;
		default:
			break;
		}
	}
}
