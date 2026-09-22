// Fill out your copyright notice in the Description page of Project Settings.

#include "FIccBattleDebuggerData.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"


void FIccBattleDebuggerData::RefreshInfo(AICC_Actor* Target)
{
	if (!Target) return;
	
	if (AICC_Player* P = Cast<AICC_Player>(Target))
	{
		TargetName = P->GetCharacterName();
		Health = P->GetRuntimeStats().CurrentHealth;
		bIsBuffed = Target->GetStatusTracker()->IsPlayerAtkBuffed();
		bIsDefBuffed = Target->GetStatusTracker()->IsPlayerDefBuffed();
		BuffCounterLabel = FString("Atk Counter: " + FString::FromInt(Target->GetStatusTracker()->GetPlayerAtkDgbCounter()) +
			"-" + "Def Counter: " + FString::FromInt(Target->GetStatusTracker()->GetDefBuffCounter()));
		CurrentBuff = "Atk: " + Target->GetStatusTracker()->DbgGetCurrentAtkBuffName() + " / " 
		+ " Def: " + Target->GetStatusTracker()->DbgGetCurrentDefBuffName();
		
	} else if (AMob* E = Cast<AMob>(Target))
	{
		TargetName = E->GetEmotionName();
		Health = E->GetStats().Health;
		bIsBuffed = Target->GetStatusTracker()->IsBuffed();
		BuffCounter = Target->GetStatusTracker()->GetBuffCounter();
		CurrentBuff = Target->GetStatusTracker()->DbgGetCurrentBuffName();
		bIsBuffed = false;
		bIsDefBuffed = false;
		BuffCounterLabel = "For Veyl Only";
	}
	
	bIsDebuffed = Target->GetStatusTracker()->IsDebuffed();
	bIsAfflicted = Target->GetStatusTracker()->IsAfflicted();
	DebuffCounter = Target->GetStatusTracker()->GetDebuffCounter();
	CurrentDebuff = Target->GetStatusTracker()->DbgGetCurrentDebuffName();
	CurrentMalus = Target->GetStatusTracker()->DbgGetCurrentMalusName();
}

void FIccDebuggerDecisionData::RefreshInfo(AICC_Actor* Target)
{
	if (!Target) return;
	
	const AMob* Emotion = Cast<AMob>(Target);
	if (!Emotion) return;
	
	TargetName = Emotion->GetEmotionName();
	
	DecisionPicked = Emotion->GetCurrentDecision();
	DecisionTable = Emotion->GetCurrentDecisionTable();
}

void FIccDebuggerTrackingData::RefreshInfo(AICC_Actor* Target)
{
	if (!Target) return;
	
	if (Cast<AICC_Player>(Target))
	{
		CurrentBuff = Target->GetStatusTracker()->DbgGetCurrentAtkBuffName();
		CurrentDebuff = Target->GetStatusTracker()->DbgGetCurrentDebuffName();
	}
	else
	{
		CurrentBuff = Target->GetStatusTracker()->DbgGetCurrentBuffName();
		CurrentDebuff = Target->GetStatusTracker()->DbgGetCurrentDebuffName();
	}
	
	
	if (AICC_Player* P = Cast<AICC_Player>(Target))
	{
		TargetName = P->GetCharacterName();
		AttackPower = static_cast<float>(P->GetRuntimeStats().AttackPower); 
		DefencePower = static_cast<float>(P->GetRuntimeStats().DefencePower);
		DamageDealt = P->GetRuntimeStats().DbgDamageDealt;
		
	} else if (AMob* E = Cast<AMob>(Target))
	{
		TargetName = E->GetEmotionName();
		AttackPower = E->GetStats().AtkPower;
		DefencePower =  E->GetStats().DefPower;
		DamageDealt = E->GetStats().DbgDamageDealt;
	}
}
