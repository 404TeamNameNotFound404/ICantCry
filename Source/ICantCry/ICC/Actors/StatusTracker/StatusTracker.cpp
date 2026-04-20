// Fill out your copyright notice in the Description page of Project Settings.
#include "StatusTracker.h"

#include "Chaos/PBDSuspensionConstraintData.h"
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
	
	DebuffCounters.Add(EDebuffStatus::DebuffAtk, 0);
	DebuffCounters.Add(EDebuffStatus::DebuffDef, 0);
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
	UStatusTracker* Tracker = Target->GetStatusTracker();
	
	if (Tracker->IsShieldActive())
	{
		PlayerShieldAccumulator--;
		
		DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " attempted to inflict " + GetDebuffName(Status) + " but fortunately a shield was used to protect from the debuff "
			+ FString::FromInt(PlayerShieldAccumulator) + " left");

		if (PlayerShieldAccumulator <= 0)
		{
			PlayerShieldAccumulator = 0;
			bIsOwnerAfflicted = false;
			bShieldBuffed = false;
			StatusCounter = 0;
			Target->ShieldDebuff(false);
			bCanDebuff = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Shield debuff protection for " + GetOwner()->GetActorLabel() + " ended");
			return;
		}
		
		return;
	}
	
	if (DebuffFlow(Status, Target))
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: Debuff neutralized by Buff; nothing applied.");
		return; 
	}
	
	// if (Tracker->IsAfflicted() || !Tracker->CanDebuff())
	// {
	// 	DebugHelper::AddMessageToLog("[Status Tracker - DeBuffS]: Emotion attempted to cast " + GetDebuffName(CurrentDebuffStatus) + " To " + Target->GetActorLabel() + " but " + GetStatusName(CurrentActiveStatus) + " has already been inflicted");
	// 	return;
	// }

	bAtkDebuffRevert = false;
	bDefDebuffRevert = false;
	
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
	
	if (DebuffCounters.Contains(Status))
	{
		DebuffCounters[Status] = 0;
		DebugHelper::AddMessageToLog("[Status Tracker]: Debuff " + GetDebuffName(Status) + " added to the counters");
	}
	else 
	{
		DebuffCounters.Add(Status, 0); 
	}
	
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
	if (!bCanBuff)
	{
		return;
	}
	
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	bAtkBuffRevert = false;
	bDefBuffRevert = false;
	// BuffFlow(BuffStatus, Target);
	
	// if (bAtkBuffRevert || bDefBuffRevert)
	// {
	// 	bIsOwnerAlreadyBuffed = false;
	// 	DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " just cancel the opposite debuff so no buff");
	// 	return; 
	// }
	
	if (BuffFlow(BuffStatus, Target))
	{
		bIsOwnerAlreadyBuffed = false;
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " cancelled debuff; buff consumed.");
		return; 
	}
	
	if (BuffCounters.Contains(BuffStatus) && CurrentBuffedStatus == BuffStatus)
	{
		BuffCounters[BuffStatus] = 0;
		return;
	}
	
	BuffCounters.Add(BuffStatus, 0);
	CurrentBuffedStatus = BuffStatus;
	bIsOwnerAlreadyBuffed = true;

	switch (BuffStatus)
	{
	case AtkBuff:
		Priority.SetPriotity(1);
		BuffAttack();
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->SimulateAura(Cast<AICC_Actor>(GetOwner()), 500.f, FColor::Red, EBuffStatus::AtkBuff);
		break;
	case DefBuff:
		Priority.SetPriotity(2);
		Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->SimulateAura(Cast<AICC_Actor>(GetOwner()), 500.f, FColor::Blue, EBuffStatus::DefBuff);
		BuffDefence();
		break;
	case LowHealth:
		Heal();
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

	DebugHelper::LogWarning("Status Counter for " + GetStatusName(CurrentActiveStatus) + " " + FString::FromInt(StatusCounter));
	DebugHelper::AddMessageToLog("[Status Tracker]: Status Counter for " + GetStatusName(CurrentActiveStatus) +  " " + FString::FromInt(StatusCounter));
	

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
		
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			Cast<AICC_Player>(Target)->GetBattleHUD()->Shoot->SetIsEnabled(true);
			Cast<AICC_Player>(Target)->Ashamed(false);
		}
		
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

	//DebuffCounter += 1;
	
	TArray<EDebuffStatus> DebuffsToRemove;
	
	for (auto& Pair : DebuffCounters)
	{
		const EDebuffStatus Status = Pair.Key;
		int32& Counter = Pair.Value;
		
		Counter++;
		
		DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " has " + 
			GetDebuffName(Status) + " at turn " + FString::FromInt(Counter));

		if (Counter >= 3)
		{
			DebuffsToRemove.Add(Status);
		}
	}
	
	for (const EDebuffStatus& S : DebuffsToRemove)
	{
		DebuffCounters.Remove(S);
	}
	
	bIsOwnerDebuffed = (DebuffCounters.Num() > 0);
	
	if (!bIsOwnerDebuffed)
	{
		bDebuffedTwice = false;
	}

	DebugHelper::LogWarning("Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
	DebugHelper::AddMessageToLog("[Status Tracker]: Status Counter for " + GetStatusName(CurrentActiveStatus) + FString::FromInt(StatusCounter));
}

void UStatusTracker::UpdateBuffStatus()
{
	if (!bIsOwnerAlreadyBuffed || !bCanBuff)
	{
		DebugHelper::AddMessageToLog("[Status Tracker]: No buff status found for " + GetOwner()->GetActorLabel());
		return;
	}

	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	TArray<EBuffStatus> BuffToRemove;
	
	for (auto& B : BuffCounters)
	{
		EBuffStatus S = B.Key; // Actual Status
		int32& C = B.Value; // Counter
		
		if (S == EBuffStatus::NoBuff)
		{
			DebugHelper::AddMessageToLog("[Status Tracker]: No buff founds for " + GetOwner()->GetActorLabel());
			continue;
		};
		
		const FString BuffName = StaticEnum<EBuffStatus>()->GetNameStringByValue(S);

		DebugHelper::AddMessageToLog(
			"[Status Tracker]: " + Target->GetActorLabel() +
			" " + BuffName + " is at turn " + FString::FromInt(C)
		);
		
		C++;
		
		// DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + 
		// 	" " + GetBuffName(S) + " is at turn " + FString::FromInt(C));
		
		if (C >= 3)
		{
			ExpireBuff(S);
			BuffToRemove.Add(S);
		}
	}
	
	for (const EBuffStatus& S : BuffToRemove)
	{
		BuffCounters.Remove(S);
	}
	
	bIsOwnerAlreadyBuffed = (BuffCounters.Num() > 0);
	
	if (!bIsOwnerAlreadyBuffed)
	{
		bBuffedTwice = false;
	}
}

static int32 FreezedUpCounter = 0;

void UStatusTracker::UnfreezeChance()
{
	const float AleatoryChance = FMath::FRand();
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	DebugHelper::LogWarning("Attempting to auto freeze");
	DebugHelper::AddMessageToLog("[Status Tracker]: Attempting to auto freeze");
	
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
	Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("You didn't manage to get rid of Freezed-Up"));

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
				FreezedUpCounter = 0;
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 25%");
				Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("You successfully got rid of Freezed-Up. You are able to shoot again"));
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
				FreezedUpCounter = 0;
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 50%");
				Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("You successfully got rid of Freezed-Up. You are able to shoot again"));
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
				FreezedUpCounter = 0;
				DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 75%");
				Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("You successfully got rid of Freezed-Up. You are able to shoot again"));
			}
			
			break;
		}

	case 4:
		{
			Target->Freeze(false);
			bIsOwnerAfflicted = false;
			CurrentActiveStatus = None;
			DebugHelper::AddMessageToLog("[Status Tracker]: Free from FreezedUp at 100% (malus ends)");
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("You successfully got rid of Freezed-Up. You are able to shoot again"));
			StatusCounter = 0;
			FreezedUpCounter = 0;
			
			break;
		}
	
	default:
		FreezedUpCounter = 0;
		break;
		
	}
	
	FTimerHandle Delay;
	GetWorld()->GetTimerManager().SetTimer(Delay, [&]
	{
		Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
	}, 1.0f, false);
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

bool UStatusTracker::IsShieldActive() const
{
	return bShieldBuffed;
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
		Emotion->GetData()->RuntimeStats.AtkPower = Emotion->GetAIMemory().InitialAttackPower;
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: " + Emotion->GetActorLabel() + " reverted it's atk power into " + FString::FromInt(Emotion->GetData()->RuntimeStats.AtkPower));
		break;
	case EBuffStatus::DefBuff:
		Emotion->GetData()->RuntimeStats.DefPower = Emotion->GetAIMemory().InitialDefencePower;
		DebugHelper::AddMessageToLog("[Status Tracker - buff flow]: " + Emotion->GetActorLabel() + " reverted it's def power into " + FString::FromInt(Emotion->GetData()->RuntimeStats.DefPower));
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
}

bool UStatusTracker::DebuffFlow(const EDebuffStatus& NewDebuffStatus, AICC_Actor* Target)
{
	UStatusTracker* Tracker = Target->GetStatusTracker();
	EBuffStatus Clash = EBuffStatus::NoBuff;
	
	const bool bIsAtk = (NewDebuffStatus == EDebuffStatus::DebuffAtk);
	const bool bHasBuffClash = (Clash != EBuffStatus::NoBuff && Tracker->BuffCounters.Contains(Clash));
	
	if (const bool bHasExistingDebuff = Tracker->DebuffCounters.Contains(NewDebuffStatus); 
		bHasExistingDebuff && !bHasBuffClash)
	{
		ResetStatsIfDebuffedTwice(Target, bIsAtk);
		return false; 
	}
	
	if (NewDebuffStatus == EDebuffStatus::DebuffAtk)
	{
		Clash = EBuffStatus::AtkBuff;
	}
	else if (NewDebuffStatus == EDebuffStatus::DebuffDef)
	{
		Clash = EBuffStatus::DefBuff;
	}
	
	if (Clash == EBuffStatus::NoBuff || !Tracker->BuffCounters.Contains(Clash))
	{
		return false; 
	}
	
	if (AICC_Player* Player = Cast<AICC_Player>(Target))
	{
		if (Clash == EBuffStatus::AtkBuff)
		{
			DebugHelper::AddMessageToLog("[Status Tracker - DebuffFlow]: Collision! " +  GetDebuffName(NewDebuffStatus) + " cancels " +
				GetBuffName(Clash) + " on " + Target->GetActorLabel());
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " got buffed so reverting");
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " Atk now is  "  + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(EBuffStatus::AtkBuff);
			bAtkDebuffRevert = true;
		}
		else if (Clash == EBuffStatus::DefBuff)
		{
			DebugHelper::AddMessageToLog("[Status Tracker - DebuffFlow]: Collision! " +  GetDebuffName(NewDebuffStatus) + " cancels " + 
				GetBuffName(Clash) + " on " + Target->GetActorLabel());
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " got buffed so reverting");
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " def now is  "  + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(EBuffStatus::DefBuff);
			bDefDebuffRevert = true;
		}
	}
	else if (const AMob* Mob = Cast<AMob>(Target))
	{
		if (Clash == EBuffStatus::AtkBuff)
		{
			DebugHelper::AddMessageToLog("[Status Tracker - DebuffFlow]: Collision! " +  GetDebuffName(NewDebuffStatus) + " cancels " +
				GetBuffName(Clash) + " on " + Target->GetActorLabel());
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " got buffed so reverting");
			Mob->GetData()->RuntimeStats.AtkPower = Mob->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " atk now is  "  + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower ));
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(EBuffStatus::AtkBuff);
			bAtkDebuffRevert = true;
			
		}
		else if (Clash == EBuffStatus::DefBuff)
		{
			DebugHelper::AddMessageToLog("[Status Tracker - DebuffFlow]: Collision! " +  GetDebuffName(NewDebuffStatus) + " cancels " +
				GetBuffName(Clash) + " on " + Target->GetActorLabel());
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " got buffed so reverting");
			Mob->GetData()->RuntimeStats.DefPower = Mob->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " + GetOwner()->GetActorLabel() + " def now is  "  + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.DefPower));
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(EBuffStatus::DefBuff);
			bDefDebuffRevert = true;
		}
	}
	
	Tracker->BuffCounters[Clash] = 0;
	Tracker->BuffCounters.Remove(Clash);
	Tracker->bIsOwnerAlreadyBuffed = (Tracker->BuffCounters.Num() > 0);

	if (Tracker->CurrentBuffedStatus == Clash)
		Tracker->CurrentBuffedStatus = EBuffStatus::NoBuff;

	DebugHelper::AddMessageToLog("[Flow]: Buff removed and stats restored for " +
		Target->GetActorLabel());
	
	return true;
}

void UStatusTracker::ResetStatsIfDebuffedTwice(AICC_Actor* Target, const bool& bAtk)
{
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		if (bAtk)
		{
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker - Stats Re-setter]: " + Target->GetActorLabel() + " got debuffed again so reverting it's stats -> atk" +
				FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		}
		else
		{
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker - Stats Re-setter]: " + Target->GetActorLabel() + " got debuffed again so reverting it's stats -> def " +
				FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
		}
	}
	else if (const AMob* Mob = Cast<AMob>(Target))
	{
		if (bAtk)
		{
			Mob->GetData()->RuntimeStats.AtkPower = Mob->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker - Stats Re-setter]: " + Mob->GetActorLabel() + " got debuffed again so reverting it's stats -> atk" +
				FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower));
		}
		else
		{
			Mob->GetData()->RuntimeStats.DefPower = Mob->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker - Stats Re-setter]: " + Mob->GetActorLabel() + " got debuffed again so reverting it's stats -> def " +
				FString::SanitizeFloat(Mob->GetData()->RuntimeStats.DefPower));
		}
	}
}

bool UStatusTracker::BuffFlow(const EBuffStatus& NewBuffStatus, AICC_Actor* Target)
{
	UStatusTracker* Tracker = Target->GetStatusTracker();
	EDebuffStatus Clash = EDebuffStatus::NoDebuff;
	
	if (NewBuffStatus == EBuffStatus::AtkBuff)
	{
		Clash = EDebuffStatus::DebuffAtk;
	}
	else if (NewBuffStatus == EBuffStatus::DefBuff)
	{
		Clash = EDebuffStatus::DebuffDef;
	}
	
	if (Clash == EDebuffStatus::NoDebuff || !Tracker->DebuffCounters.Contains(Clash))
	{
		return false; 
	}
	
	if (Clash != EDebuffStatus::NoDebuff && Tracker->DebuffCounters.Contains(Clash))
	{
		return false; 
	}
	
	DebugHelper::AddMessageToLog("[Status Tracker - BuffFlow]: Collision! " + GetBuffName(NewBuffStatus) + " neutralized " + GetDebuffName(Clash));
	ExpireBuff(NewBuffStatus, Target); 
	Tracker->DebuffCounters.Remove(Clash);
	Tracker->bIsOwnerDebuffed = (Tracker->DebuffCounters.Num() > 0);
	if (Tracker->CurrentDebuffStatus == Clash) Tracker->CurrentDebuffStatus = EDebuffStatus::NoDebuff;
	
	Tracker->DebuffCounters[Clash] = 0;
	Tracker->DebuffCounters.Remove(Clash);
	
	Tracker->bIsOwnerDebuffed = (Tracker->DebuffCounters.Num() > 0);

	if (Tracker->CurrentDebuffStatus == Clash)
	{
		Tracker->CurrentDebuffStatus = EDebuffStatus::NoDebuff;
	}
	
	DebugHelper::AddMessageToLog("[Status Tracker - BuffFlow]: Debuff removed and stats restored for " + Target->GetActorLabel());
	
	return true;
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
	StatusCounter = 0;
	BuffStatusCounter = 0;
	DebuffCounter = 0;
	CurrentBuffedStatus = EBuffStatus::NoBuff;
	CurrentActiveStatus = EAfflictedStatus::None;
	CurrentDebuffStatus = EDebuffStatus::NoDebuff;
	DebugHelper::LogMessage(8, FColor::Blue, "[Status Tracker]: Stats successfully restored");
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
		const int32 BuffedAtk = FMath::FloorToInt( Mob->GetData()->RuntimeStats.AtkPower * Mob->GetBattleData()->EmotionAtkBuffIncrement);
		Mob->GetData()->RuntimeStats.AtkPower += BuffedAtk;

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
		Mob->GetData()->RuntimeStats.DefPower += FMath::FloorToInt(Mob->GetData()->RuntimeStats.DefPower * Mob->GetBattleData()->EmotionDefBuffIncrement);
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Mob->GetActorLabel() + " buffed it's def " + FString::FromInt(Mob->GetData()->RuntimeStats.DefPower));

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
	bShieldBuffed = true;
	bCanDebuff = false;
	PlayerShieldAccumulator = 3;
	
	if (Target->IsA(AICC_Player::StaticClass()))
	{
		DebugHelper::LogWarning("[Status Tracker]: " + Target->GetActorLabel() + " uses shield debuff. Shield left " + FString::FromInt(PlayerShieldAccumulator) + 
			"\nStatus Tracker]: shield debuff and candebuff for "  + Target->GetActorLabel() + " is " + FString(bShieldBuffed ? "True" : "False") + " " + FString(bCanDebuff ? "True" : "False"));
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " uses shield debuff. Shield left " + FString::FromInt(PlayerShieldAccumulator));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
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

void UStatusTracker::ExpireBuff(const EBuffStatus& ExpiredTarget)
{
	const AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());
	
	switch (ExpiredTarget)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			DebugHelper::AddMessageToLog("[Status Tracker]: Player buff before returning back to normal " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Player Buff ended atk returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(ExpiredTarget);
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.AtkPower));
			Emotion->GetData()->RuntimeStats.AtkPower = Emotion->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, atk returns to " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.AtkPower));
			PerkData.bBuffAtk = false;
			bIsOwnerAlreadyBuffed = false;
			bCanBuff = true;
			bBuffedTwice = false;
			Emotion->GetBattleHandler()->DeactivateAura(ExpiredTarget);
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff ended def returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(ExpiredTarget);
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.DefPower));
			Emotion->GetData()->RuntimeStats.DefPower = Emotion->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, def returns to " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.DefPower));
			bIsOwnerAlreadyBuffed = false;
			PerkData.bBuffDef = false;
			bCanBuff = true;
			bBuffedTwice = false;
			Emotion->GetBattleHandler()->DeactivateAura(ExpiredTarget);
		}
		break;
	case LowHealth:
		bIsOwnerAlreadyBuffed = false;
		PerkData.bLowHealth = false;
		bCanBuff = true;
		bBuffedTwice = false;
		break;
	case NoBuff:
		bIsOwnerAlreadyBuffed = false;
		bBuffedTwice = false;
		break;
	default:
		break;
	}
}

void UStatusTracker::ExpireBuff(const EBuffStatus& ExpiredBuff, AICC_Actor* Target)
{
	switch (ExpiredBuff)
	{
	case AtkBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			DebugHelper::AddMessageToLog("[Status Tracker]: Player buff before returning back to normal " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			Instance->GetRuntimeStats().AttackPower = Instance->GetPersistentData()->InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Player Buff ended atk returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
			
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(ExpiredBuff);
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.AtkPower));
			Emotion->GetData()->RuntimeStats.AtkPower = Emotion->GetAIMemory().InitialAttackPower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, atk returns to " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.AtkPower));
			PerkData.bBuffAtk = false;
			bIsOwnerAlreadyBuffed = false;
			bCanBuff = true;
			bBuffedTwice = false;
			Emotion->GetBattleHandler()->DeactivateAura(ExpiredBuff);
		}
		break;
	case DefBuff:
		if (Target->IsA(AICC_Player::StaticClass()))
		{
			AICC_Player* Player = Instance->GetCurrentPlayer();
			Instance->GetRuntimeStats().DefencePower = Instance->GetPersistentData()->InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: Buff ended def returns to " + FString::SanitizeFloat(Instance->GetRuntimeStats().DefencePower));
			bIsOwnerAlreadyBuffed = false;
			bCanDebuff = true;
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->DeactivateAura(ExpiredBuff);
		}
		if (Target->IsA(AMob::StaticClass()))
		{
			const AMob* Emotion = Cast<AMob>(GetOwner());
			DebugHelper::AddMessageToLog("[Status Tracker]:" + Target->GetActorLabel() + " before returning back to normal " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.DefPower));
			Emotion->GetData()->RuntimeStats.DefPower = Emotion->GetAIMemory().InitialDefencePower;
			DebugHelper::AddMessageToLog("[Status Tracker]: " +  Target->GetActorLabel() +  " Buff ended, def returns to " + FString::SanitizeFloat(Emotion->GetData()->RuntimeStats.DefPower));
			bIsOwnerAlreadyBuffed = false;
			PerkData.bBuffDef = false;
			bCanBuff = true;
			bBuffedTwice = false;
			Emotion->GetBattleHandler()->DeactivateAura(ExpiredBuff);
		}
		break;
	case LowHealth:
		bIsOwnerAlreadyBuffed = false;
		PerkData.bLowHealth = false;
		bCanBuff = true;
		bBuffedTwice = false;
		break;
	case NoBuff:
		bIsOwnerAlreadyBuffed = false;
		bBuffedTwice = false;
		break;
	default:
		break;
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
	AICC_Actor* Target = Cast<AICC_Actor>(GetOwner());

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		const AICC_Player* Player = Cast<AICC_Player>(Target);
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math :starting Debuff atk" + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower));
		Instance->GetRuntimeStats().AttackPower -= Instance->GetRuntimeStats().AttackPower * Player->GetBattleData()->DebuffAtkMalus;
		DebugHelper::AddMessageToLog("[Status Tracker]: Player debuff math : Debuff atk post malus -> " + FString::SanitizeFloat(Instance->GetRuntimeStats().AttackPower) +
			"\n[Status Tracker]: Atk malus dealed -> " + FString::SanitizeFloat(Player->GetBattleData()->DebuffAtkMalus));
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		const AMob* Mob = Cast<AMob>(Target);
		Mob->GetData()->RuntimeStats.AtkPower -= Mob->GetData()->RuntimeStats.AtkPower * Mob->GetBattleData()->EmotionAtkDebuffMalus;
		DebugHelper::LogSuccess("[Status Tracker]: " +Mob->GetActorLabel() + "got it's atk de-buffed now has " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: AI atk value (debuff) " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower));

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
		Mob->GetData()->RuntimeStats.AtkPower -= Mob->GetData()->RuntimeStats.AtkPower * Mob->GetBattleData()->EmotionAtkDebuffMalus;
		DebugHelper::LogSuccess("[Status Tracker]: " +Mob->GetActorLabel() + "got it's atk de-buffed now has " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: AI atk value (debuff) " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.AtkPower));

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
		Mob->GetData()->RuntimeStats.DefPower -= Mob->GetData()->RuntimeStats.DefPower * Mob->GetBattleData()->EmotionDefDebuffMalus;
		
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
		const AMob* Mob = Cast<AMob>(Target);
		Mob->GetData()->RuntimeStats.DefPower -= Mob->GetData()->RuntimeStats.DefPower * Mob->GetBattleData()->EmotionDefDebuffMalus;
		DebugHelper::LogSuccess("[Status Tracker]: " +Mob->GetActorLabel() + "got it's def de-buffed now has " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.DefPower));
		DebugHelper::AddMessageToLog("[Status Tracker]: AI def value (debuff) " + FString::SanitizeFloat(Mob->GetData()->RuntimeStats.DefPower));

		switch (Mob->GetMobType())
		{
		case MobAnger:
			PerkData.bDebuffDef = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			break;
		case MobShame:
			break;
		case MobJoy:
			break;
		case MobDisgust:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			PerkData.bDebuffDef = true;
			break;
		case MobFear:
			break;
		case MobJealousy:
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now debuff def");
			PerkData.bDebuffDef = true;
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
			Emotion->GetData()->RuntimeStats.AtkPower = Emotion->GetAIMemory().InitialAttackPower;
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
			Emotion->GetData()->RuntimeStats.DefPower = Emotion->GetAIMemory().InitialDefencePower;
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
	bIsOwnerAfflicted = true;
	DebugHelper::LogMessage(5, FColor::FromHex("FE7743"), Target->GetActorLabel() + " can't perform attack");
	DebugHelper::AddMessageToLog("[Status Tracker]: " + Target->GetActorLabel() + " can't perform attack");

	if (Target->IsA(AICC_Player::StaticClass()))
	{
		AICC_Player* Player = Cast<AICC_Player>(Target);
		Player->Ashamed(true);
	}

	if (Target->IsA(AMob::StaticClass()))
	{
		AMob* Mob = Cast<AMob>(Target);
		Mob->Ashamed(true);
		
		DebugHelper::AddMessageToLog("[Status Tracker]: " + Mob->GetActorLabel() + " ashamed");
		DebugHelper::LogMessage(8, FColor::White, "[Status Tracker]: " + Mob->GetActorLabel() + " ashamed");
		
		switch ( Mob->GetMobType())
		{
		case MobAnger:
		case MobJoy:
		case MobDisgust:
		case MobFear:
		case MobJealousy:
		case MobSadness:
			PerkData.Clear();
			PerkData.bAshamed = true;
			DebugHelper::AddMessageToLog("[Status Tracker]: Decision table of " + Mob->GetActorLabel() + " is now ashamed");
			break;
		case MobShame:
		case MobAnxiety:
		case MobCalm:
		default:
			break;
		}
	}
}
