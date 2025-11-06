#include "DecisionMaker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

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
	// const float Chance = FMath::FRand(); // Random float between 0.0 and 1.0
	// float Counter = 0.0f;
	// float TotalWeight = 0.0f;
	//
	// DebugHelper::LogMessage(6, FColor::Purple, "Chance extracted: " + FString::SanitizeFloat(Chance));
	//
	// TMap<EDecision, float> FilteredMap;
	//
	//
	// for (const auto& Entry : DecisionMap)
	// {
	// 	if (Entry.Key != LastDecision || CanRepeat(Entry.Key))
	// 	{
	// 		FilteredMap.Add(Entry.Key, Entry.Value);
	// 	}
	// }
	//
	// const TMap<EDecision, float>& FinalMap = FilteredMap.Num() > 0 ? FilteredMap : DecisionMap;
	//
	//
	// for (const auto& Entry : FinalMap)
	// {
	// 	TotalWeight += Entry.Value;
	// }
	//
	// if (TotalWeight <= 0.0f)
	// {
	// 	DebugHelper::LogWarning("Total weight is zero or negative. Returning default decision.");
	// 	return EDecision::None;
	// }
	//
	// for (const auto& Entry : FinalMap)
	// {
	// 	const float NormalizedWeight = Entry.Value / TotalWeight;
	// 	Counter += NormalizedWeight;
	// 	
	// 	DebugHelper::LogMessage(6, FColor::Magenta, "Decision " + UEnum::GetValueAsString(Entry.Key) + "| Normalized weight: " + FString::SanitizeFloat(NormalizedWeight) + " | Counter: " + FString::SanitizeFloat(Counter));
	//
	// 	if (Chance < Counter)
	// 	{
	// 		LastDecision = Entry.Key;
	// 		return Entry.Key;
	// 	}
	// }
	//
	// return EDecision::None;

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

