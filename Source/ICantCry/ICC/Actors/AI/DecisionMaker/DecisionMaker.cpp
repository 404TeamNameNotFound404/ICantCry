#include "DecisionMaker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

int32 FDecisionPriorities::GetPriority(const EDecision& Decision) const
{
	switch (Decision)
	{
	case EDecision::BuffItSelf:         return BuffAtk;
	case EDecision::DebuffAtk:          return DebuffAtk;
	case EDecision::BuffDefence:        return BuffDef;
	case EDecision::DebuffDefence:      return DebuffDef;
	case EDecision::None:               return None;
	case EDecision::EnvyBurned:         return EnvyBurned;
	case EDecision::DebuffShieldItSelf: return DebuffShield;
	default: return None;;
	}
}

EDecision FDecisionPriorities::PickHighestDecision(const TArray<EDecision>& Decisions) const
{
	if (Decisions.Num() == 0)
		return EDecision::None;

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
	using DecisionPopulator = TFunction<void(FDecisionMaker&, const UBattleData*)>;

	static const TMap<EMobType, DecisionPopulator> EmotionDecisionMap = {
    { EMobType::MobAnger, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::BuffItSelf, BD->BuffAtkChance);
        DM.DecisionMap.Add(EDecision::None, BD->AngerNormalAttackChance);
    }},
    { EMobType::MobJoy, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::HealItSelf, BD->HealItselfChance);
        DM.DecisionMap.Add(EDecision::HealOther, BD->HealOtherChance);
        DM.DecisionMap.Add(EDecision::None, BD->JoyNormalAttackChance);
    }},
    { EMobType::MobSadness, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffDefence, BD->SadnessDebuffDefChance);
        DM.DecisionMap.Add(EDecision::None, BD->SadnessNormalAttackChance);
    }},
    { EMobType::MobDisgust, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, BD->DisgustDebuffAtkChance);
        DM.DecisionMap.Add(EDecision::None, BD->DisgustNormalAttackChance);
    }},
    { EMobType::MobFear, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::BuffDefence, BD->FearBuffDefChance);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence, BD->FearBuffOtherDefChance);
        DM.DecisionMap.Add(EDecision::None, BD->FearNormalAttackChance);
    }},
    { EMobType::MobAnxiety, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, BD->AnxietyDebuffAtkChance);
        DM.DecisionMap.Add(EDecision::DebuffDefence, BD->AnxietyDebuffDefChance);
        DM.DecisionMap.Add(EDecision::FreezedUp, BD->FreezedUpChance);
    }},
    { EMobType::MobJealousy, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::EnvyBurned, BD->EnvyBurnedChance);
        DM.DecisionMap.Add(EDecision::BuffOther, BD->BuffOtherAtkChance);
        DM.DecisionMap.Add(EDecision::None, BD->JealousyNormalAttackChance);
    }},
    { EMobType::MobCalm, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffShieldItSelf, BD->DebuffShieldItselfChance);
        DM.DecisionMap.Add(EDecision::DebuffShieldOther, BD->DebuffShieldOtherChance);
        DM.DecisionMap.Add(EDecision::BuffDefence, BD->CalmBuffDefChance);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence, BD->CalmBuffOtherDefChance);
    }},
};

	DecisionMap.Empty(); 

	UBattleData* BattleData = Current->GetBattleData();
	checkf(BattleData, TEXT("Battle data appears to be invalid"));

	EMobType MobType = Current->GetMobType(); 

	if (const DecisionPopulator* Populator = EmotionDecisionMap.Find(MobType))
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
	TMap<EDecision, float> FilteredMap;
	for (auto& Entry : DecisionMap)
	{
		if (Entry.Key != LastDecision || CanRepeat(Entry.Key))
			FilteredMap.Add(Entry.Key, Entry.Value);
	}
	
	if (FilteredMap.Num() == 0)
	{
		FilteredMap = DecisionMap;
	}
		
	
	float TotalWeight = 0.f;
	
	for (auto& Entry : FilteredMap)
	{
		TotalWeight += Entry.Value;
	}
		
	
	if (TotalWeight <= KINDA_SMALL_NUMBER)
	{
		for (auto& Entry : FilteredMap)
		{
			if (Entry.Key != EDecision::None)
				return Entry.Key;
		}

		
		return EDecision::None;
	}
	
	const float Chance = FMath::FRand();
	float Counter = 0.f;

	DebugHelper::AddMessageToLog("Chance Extracted : " + FString::SanitizeFloat(Chance));

	for (auto& Entry : FilteredMap)
	{
		Counter += Entry.Value / TotalWeight;
		if (Chance <= Counter)
		{
			LastDecision = Entry.Key;
			return Entry.Key;
		}
	}
	
	for (auto& Entry : FilteredMap)
	{
		if (Entry.Key != EDecision::None)
		{
			LastDecision = Entry.Key;
			return Entry.Key;
		}
	}
	
	return EDecision::None;
}


EDecision FDecisionMaker::EnhancedThought(AMob* Emotion)
{
	const FDecisionPriorities Priorities;

	TArray<EDecision> ActiveDecisions;

	DebugHelper::AddMessageToLog("Enhanced thought started!");
	
	if (Emotion->GetIsIsBuffedAtk()) ActiveDecisions.Add(EDecision::BuffItSelf);
	if (Emotion->GetPlayerDebuffAttack()) ActiveDecisions.Add(EDecision::DebuffAtk);
	if (Emotion->GetIsBuffedDefence()) ActiveDecisions.Add(EDecision::BuffDefence);
	if (Emotion->GetIsTargetDefenceDebuffed()) ActiveDecisions.Add(EDecision::DebuffDefence);
	if (Emotion->GetIsIsLow()) ActiveDecisions.Add(EDecision::HealItSelf);
	if (Emotion->GetIsIsEnvyBurned()) ActiveDecisions.Add(EDecision::EnvyBurned);
	if (Emotion->GetIsDebuffShield()) ActiveDecisions.Add(EDecision::DebuffShieldItSelf);

	DebugHelper::AddMessageToLog("Checking " + Emotion->GetActorLabel() + " buff / debuff");
	DebugHelper::LogWarning("Thinking");
	
	if (ActiveDecisions.Num() > 0)
	{
		DebugHelper::AddMessageToLog(Emotion->GetActorLabel() + " decided over priority");
		return Priorities.PickHighestDecision(ActiveDecisions);
	}

	DebugHelper::AddMessageToLog("Enhanced thought complete!");
	
	return Thought();
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

