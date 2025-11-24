#include "DecisionMaker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

int32 FDecisionPriorities::GetPriority(const EDecision& Decision) const
{
	switch (Decision)
	{
	case EDecision::BuffItSelf:         return AtkBuff;
	case EDecision::DebuffAtk:          return DebuffAtk;
	case EDecision::BuffDefence:        return DefBuff;
	case EDecision::DebuffDefence:      return DebuffDef;
	case EDecision::None:               return None;
	case EDecision::EnvyBurned:         return EnvyBurned;
	case EDecision::DebuffShieldItSelf: return DebuffShield;
	default: return None;;
	}
}

EDecision FDecisionPriorities::PickHighestDecision(const TArray<EDecision>& Decisions) const
{
	if (Decisions.Num() == 0) return EDecision::None;

	EDecision Top = Decisions[0];
	for (int i = 1; i < Decisions.Num(); i++)
	{
		if (GetPriority(Decisions[i]) > GetPriority(Top))
			Top = Decisions[i];
	}
	return Top;
}

FDecisionMaker::FDecisionMaker()
{
	FMath::RandInit(FPlatformTime::Cycles());
}

void FDecisionMaker::Setup(AMob* Current)
{
	using FDecisionPopulator = TFunction<void(FDecisionMaker&, const UBattleData*)>;

	// Experimental : decision weight
	FDecisionWeight EmotionDecisionWeight;
	if (Current->GetIsIsBuffedAtk())           
		EmotionDecisionWeight = Current->GetDecisionTable()->BuffAtk;
	else if (Current->GetPlayerDebuffAttack()) 
		EmotionDecisionWeight = Current->GetDecisionTable()->DebuffAtk;
	else if (Current->GetIsIsLow())            
		EmotionDecisionWeight = Current->GetDecisionTable()->LowHealth;
	else if (Current->GetIsIsEnvyBurned())   
		EmotionDecisionWeight = Current->GetDecisionTable()->EnvyBurned;
	else                                     
		EmotionDecisionWeight = Current->GetDecisionTable()->Idle;    
	// Delete the part inside the comments

	static const TMap<EMobType, FDecisionPopulator> EmotionDecisionMap = {
    { EMobType::MobAnger, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::BuffItSelf, EmotionDecisionWeight.BuffAtkWeight /*BD->BuffAtkChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight /*BD->AngerNormalAttackChance*/);
    }},
    { EMobType::MobJoy, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::HealItSelf, EmotionDecisionWeight.HealWeight /*BD->HealItselfChance*/);
        DM.DecisionMap.Add(EDecision::HealOther, EmotionDecisionWeight.HealOtherWeight /*BD->HealOtherChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight /*BD->JoyNormalAttackChance*/);
    }},
    { EMobType::MobSadness, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffDefence, EmotionDecisionWeight.DebuffDefWeight/*BD->SadnessDebuffDefChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight /*BD->SadnessNormalAttackChance*/);
    }},
    { EMobType::MobDisgust, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, EmotionDecisionWeight.DebuffAtkWeight /*BD->DisgustDebuffAtkChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight /*BD->DisgustNormalAttackChance*/);
    }},
    { EMobType::MobFear, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::BuffDefence, EmotionDecisionWeight.BuffDefWeight /*BD->FearBuffDefChance*/);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence,EmotionDecisionWeight.BuffOtherDefWeight /*BD->FearBuffOtherDefChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight /*BD->FearNormalAttackChance*/);
    }},
    { EMobType::MobAnxiety, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, EmotionDecisionWeight.DebuffAtkWeight/*BD->AnxietyDebuffAtkChance*/);
        DM.DecisionMap.Add(EDecision::DebuffDefence,EmotionDecisionWeight.DebuffDefWeight /*BD->AnxietyDebuffDefChance*/);
        DM.DecisionMap.Add(EDecision::FreezedUp, EmotionDecisionWeight.FreezedUpWeight/*BD->FreezedUpChance*/);
    }},
    { EMobType::MobJealousy, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::EnvyBurned, EmotionDecisionWeight.EnvyBurnedWeight /*BD->EnvyBurnedChance*/);
        DM.DecisionMap.Add(EDecision::BuffOther, EmotionDecisionWeight.BuffOtherAtkWeight/*BD->BuffOtherAtkChance*/);
        DM.DecisionMap.Add(EDecision::None, EmotionDecisionWeight.AttackWeight/*BD->JealousyNormalAttackChance*/);
    }},
    { EMobType::MobCalm, [&EmotionDecisionWeight](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffShieldItSelf, EmotionDecisionWeight.ShieldWeight/*BD->DebuffShieldItselfChance*/);
        DM.DecisionMap.Add(EDecision::DebuffShieldOther, EmotionDecisionWeight.ShieldOtherWeight/*BD->DebuffShieldOtherChance*/);
        DM.DecisionMap.Add(EDecision::BuffDefence, EmotionDecisionWeight.BuffDefWeight /*BD->CalmBuffDefChance*/);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence, EmotionDecisionWeight.BuffOtherDefWeight /*BD->CalmBuffOtherDefChance*/);
    }},
	};

	DecisionMap.Empty(); 

	UBattleData* BattleData = Current->GetBattleData();
	checkf(BattleData, TEXT("Battle data appears to be invalid"));

	EMobType MobType = Current->GetMobType(); 

	if (const FDecisionPopulator* Populator = EmotionDecisionMap.Find(MobType))
	{
		(*Populator)(*this, BattleData);
	}
	else
	{
		DebugHelper::LogWarning(FString::Printf( TEXT("No decision logic found for MobType: %d"), static_cast<int32>(MobType)));
	}
}

EDecision FDecisionMaker::Thought()
{
	TArray<TPair<EDecision, float>> Decisions;
	Decisions.Reserve(DecisionMap.Num());
	for (const auto& Elem : DecisionMap)
	{
		Decisions.Add({Elem.Key, Elem.Value});
	}
	
	Decisions.Sort([](const auto& A, const auto& B)
	{
		return static_cast<uint8>(A.Key) < static_cast<uint8>(B.Key);
	});
	
	TArray<TPair<EDecision, float>> Filtered;
	for (const auto& Entry : Decisions)
	{
		if (Entry.Key != LastDecision || CanRepeat(Entry.Key))
		{
			Filtered.Add(Entry);
		}
	}

	if (Filtered.Num() == 0)
	{
		Filtered = Decisions;
	}

	float TotalWeight = 0.f;
	for (const auto& Entry : Filtered)
	{
		TotalWeight += Entry.Value;
	}

	if (TotalWeight <= KINDA_SMALL_NUMBER)
	{
		for (const auto& Entry : Filtered)
		{
			if (Entry.Key != EDecision::Invalid)
				return Entry.Key;
		}

		return EDecision::None;
	}

	
	const float Chance = FMath::FRand();
	float WeightAccumulator = 0.f;

	for (const auto& Entry : Filtered)
	{
		WeightAccumulator += Entry.Value / TotalWeight;
		if (Chance <= WeightAccumulator)
		{
			LastDecision = Entry.Key;
			return Entry.Key;
		}
	}

	return EDecision::None;
}


EDecision FDecisionMaker::EnhancedThought(AMob* Emotion)
{
	if (!Emotion || !Emotion->GetDecisionTable()) return Thought();

	// Decide current state by value
	FDecisionWeight CurrentWeights;
	if (Emotion->GetIsIsBuffedAtk())  CurrentWeights = Emotion->GetDecisionTable()->BuffAtk;
	else if (Emotion->GetPlayerDebuffAttack()) CurrentWeights = Emotion->GetDecisionTable()->DebuffAtk;
	else if (Emotion->GetIsIsLow())           CurrentWeights = Emotion->GetDecisionTable()->LowHealth;
	else if (Emotion->GetIsIsEnvyBurned())  CurrentWeights = Emotion->GetDecisionTable()->EnvyBurned;
	else                                    CurrentWeights = Emotion->GetDecisionTable()->Idle;

	// Build active decisions based on current state
	TArray<EDecision> ActiveDecisions;
	if (CurrentWeights.BuffAtkWeight > 0.f)       ActiveDecisions.Add(EDecision::BuffItSelf);
	if (CurrentWeights.DebuffAtkWeight > 0.f)     ActiveDecisions.Add(EDecision::DebuffAtk);
	if (CurrentWeights.EnvyBurnedWeight > 0.f)    ActiveDecisions.Add(EDecision::EnvyBurned);
	if (CurrentWeights.LowHealthWeight > 0.f)     ActiveDecisions.Add(EDecision::HealItSelf);
	if (CurrentWeights.BuffDefWeight > 0.f)       ActiveDecisions.Add(EDecision::BuffDefence);
	if (CurrentWeights.DebuffDefWeight > 0.f)     ActiveDecisions.Add(EDecision::DebuffDefence);
	if (CurrentWeights.ShieldWeight > 0.f)        ActiveDecisions.Add(EDecision::DebuffShieldItSelf);

	// Use the FDecisionWeight as priorities
	FDecisionPriorities Priorities(CurrentWeights);

	return Priorities.PickHighestDecision(ActiveDecisions);
}


void FDecisionMaker::Clear()
{
	DecisionMap.Empty();
}

void FDecisionMaker::ResetDecision()
{
	LastDecision = EDecision::Invalid;
}

bool FDecisionMaker::CanRepeat(const EDecision& Decision)
{
	if (Decision == EDecision::None)
	{
		return true;
	}
	
	return false;
}

