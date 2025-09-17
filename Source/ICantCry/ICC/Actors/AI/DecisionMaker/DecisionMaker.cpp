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
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobJoy, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::HealItSelf, BD->HealItselfChance);
        DM.DecisionMap.Add(EDecision::HealOther, BD->HealOtherChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobSadness, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffDefence, BD->DebuffDefChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobDisgust, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, BD->DebuffAtkChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobFear, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::BuffDefence, BD->BuffDefChance);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence, BD->BuffOtherDefChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobAnxiety, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffAtk, BD->DebuffAtkChance);
        DM.DecisionMap.Add(EDecision::DebuffDefence, BD->DebuffDefChance);
        DM.DecisionMap.Add(EDecision::FreezedUp, BD->FreezedUpChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobJealousy, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::EnvyBurned, BD->EnvyBurnedChance);
        DM.DecisionMap.Add(EDecision::BuffOther, BD->BuffOtherAtkChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
    }},
    { EMobType::MobCalm, [](FDecisionMaker& DM, const UBattleData* BD) {
        DM.DecisionMap.Add(EDecision::DebuffShieldItSelf, BD->DebuffShieldItselfChance);
        DM.DecisionMap.Add(EDecision::DebuffShieldOther, BD->DebuffShieldOtherChance);
        DM.DecisionMap.Add(EDecision::BuffDefence, BD->BuffDefChance);
        DM.DecisionMap.Add(EDecision::BuffOtherDefence, BD->BuffOtherDefChance);
        DM.DecisionMap.Add(EDecision::None, BD->NormalAttackChance);
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

	const float Chance = FMath::FRand(); // 0.0 - 1.0
    float Counter = 0.0f;
    float TotalWeight = 0.0f;
	
    TMap<EDecision, float> FilteredMap;
    for (const auto& Entry : DecisionMap)
    {
        if (Entry.Key != LastDecision || CanRepeat(Entry.Key))
            FilteredMap.Add(Entry.Key, Entry.Value);
    }

   
    const TMap<EDecision, float>& FinalMap = FilteredMap.Num() > 0 ? FilteredMap : DecisionMap;
	
    for (const auto& Entry : FinalMap)
        TotalWeight += Entry.Value;

    if (TotalWeight <= KINDA_SMALL_NUMBER)
    {
        for (const auto& Entry : DecisionMap)
        {
            if (Entry.Key != EDecision::None)
            {
                LastDecision = Entry.Key;
                return Entry.Key;
            }
        }
        return EDecision::None;
    }
	
    for (const auto& Entry : FinalMap)
    {
        const float NormalizedWeight = Entry.Value / TotalWeight;
        Counter += NormalizedWeight;

        DebugHelper::LogMessage(6, FColor::Magenta, "Decision " + UEnum::GetValueAsString(Entry.Key));

        if (Chance < Counter)
        {
            LastDecision = Entry.Key;
            return Entry.Key;
        }
    }
	
    for (const auto& Entry : FinalMap)
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

