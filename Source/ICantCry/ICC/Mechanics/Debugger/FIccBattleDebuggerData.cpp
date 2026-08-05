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
	} else if (AMob* E = Cast<AMob>(Target))
	{
		TargetName = E->GetEmotionName();
		Health = E->GetStats().Health;
	}
		
	bIsBuffed = Target->GetStatusTracker()->IsBuffed();
	bIsDebuffed = Target->GetStatusTracker()->IsDebuffed();
	bIsAfflicted = Target->GetStatusTracker()->IsAfflicted();
	BuffCounter = Target->GetStatusTracker()->GetBuffCounter();
	DebuffCounter = Target->GetStatusTracker()->GetDebuffCounter();
	CurrentBuff = Target->GetStatusTracker()->DbgGetCurrentBuffName();
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
	
	CurrentBuff = Target->GetStatusTracker()->DbgGetCurrentBuffName();
	CurrentDebuff = Target->GetStatusTracker()->DbgGetCurrentDebuffName();
	
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
