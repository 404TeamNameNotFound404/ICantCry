#include "DecisionMaker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
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
	 DecisionMap.Empty();

    if (!Current || !Current->GetDecisionTable())
        return;
	
    FDecisionWeight CurrentWeights;

    if (Current->GetStatusTracker()->GetPerkData().bBuffAtk)
    {
	    CurrentWeights = Current->GetDecisionTable()->BuffAtk;
    	DebugHelper::AddMessageToLog("Decision table used: Buffed Atk");
    }
    else if (Current->GetStatusTracker()->GetPerkData().bDebuffAtk)
    {
	    CurrentWeights = Current->GetDecisionTable()->DebuffAtk;
    	DebugHelper::AddMessageToLog("Decision table used: Debuff Atk");
    }
    else if (Current->GetStatusTracker()->GetPerkData().bLowHealth)
    {
	    CurrentWeights = Current->GetDecisionTable()->LowHealth;
    	DebugHelper::AddMessageToLog("Decision table used: Low health");
    }
    else if (Current->GetStatusTracker()->GetPerkData().bEnvyBurned)
    {
	    CurrentWeights = Current->GetDecisionTable()->EnvyBurned;
    	DebugHelper::AddMessageToLog("Decision table used: Envy burned");
    }
	else if (Current->GetStatusTracker()->GetPerkData().bBuffDef)
	{
		CurrentWeights = Current->GetDecisionTable()->BuffDef;
		DebugHelper::AddMessageToLog("Decision table used: Buff Def");
	}
	else if (Current->GetStatusTracker()->GetPerkData().bDebuffDef)
	{
		CurrentWeights = Current->GetDecisionTable()->DebuffDef;
		DebugHelper::AddMessageToLog("Decision table used: Debuff Def");
	}
	else if (Current->GetStatusTracker()->GetPerkData().bAshamed)
	{
		CurrentWeights = Current->GetDecisionTable()->Ashamed;
		DebugHelper::AddMessageToLog("Decision table used: Ashamed");
	}
    else
    {
	    CurrentWeights = Current->GetDecisionTable()->Idle;
    	DebugHelper::AddMessageToLog("Decision table used: Normal atk");
    }

	switch (Current->GetMobType())
	{
	case MobAnger:
		if (CurrentWeights.BuffAtkWeight > 0.f) DecisionMap.Add(EDecision::BuffItSelf, CurrentWeights.BuffAtkWeight);
		if (CurrentWeights.AttackWeight > 0.f)  DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		break;
	case MobShame:
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		break;
    case MobJoy:
		if (CurrentWeights.HealWeight > 0.f) DecisionMap.Add(EDecision::HealItSelf, CurrentWeights.HealWeight);
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.HealOtherWeight > 0.f) DecisionMap.Add(EDecision::HealOther, CurrentWeights.HealOtherWeight);
		break;
	case MobDisgust:
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.DebuffAtkWeight > 0.f) DecisionMap.Add(EDecision::DebuffAtk, CurrentWeights.DebuffAtkWeight);
		break;
	case MobFear:
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.BuffDefWeight > 0.f) DecisionMap.Add(EDecision::BuffDefence, CurrentWeights.BuffDefWeight);
		if (CurrentWeights.BuffOtherDefWeight > 0.f) DecisionMap.Add(EDecision::BuffOtherDefence, CurrentWeights.BuffOtherDefWeight);
		break;
	case MobJealousy:
		if (CurrentWeights.EnvyBurnedWeight > 0.f) DecisionMap.Add(EDecision::EnvyBurned, CurrentWeights.EnvyBurnedWeight);
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.BuffOtherAtkWeight > 0.f) DecisionMap.Add(EDecision::BuffOther, CurrentWeights.BuffOtherAtkWeight);
		break;
	case MobSadness:
		if (CurrentWeights.AttackWeight > 0.f) DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.DebuffDefWeight > 0.f) DecisionMap.Add(EDecision::DebuffDefence, CurrentWeights.DebuffDefWeight);
		break;
	case MobAnxiety:
		if (CurrentWeights.FreezedUpWeight > 0.f) DecisionMap.Add(EDecision::FreezedUp, CurrentWeights.FreezedUpWeight);
		if (CurrentWeights.DebuffDefWeight > 0.f) DecisionMap.Add(EDecision::DebuffDefence, CurrentWeights.DebuffDefWeight);
		if (CurrentWeights.DebuffAtkWeight > 0.f) DecisionMap.Add(EDecision::DebuffAtk, CurrentWeights.DebuffAtkWeight);
 		break;
	case MobCalm:
		if (CurrentWeights.ShieldWeight > 0.f )  DecisionMap.Add(EDecision::DebuffShieldItSelf, CurrentWeights.ShieldWeight);
		if (CurrentWeights.ShieldOtherWeight > 0.f )DecisionMap.Add(EDecision::DebuffShieldOther, CurrentWeights.ShieldOtherWeight);
		if (CurrentWeights.BuffDefWeight > 0.f) DecisionMap.Add(EDecision::BuffDefence, CurrentWeights.BuffDefWeight);
		if (CurrentWeights.BuffOtherDefWeight > 0.f) DecisionMap.Add(EDecision::BuffOtherDefence, CurrentWeights.BuffOtherDefWeight);
		break;
    default:
    	break;
	}
}

EDecision FDecisionMaker::Thought()
{
	DebugHelper::AddMessageToLog("AI is thinking the best move..");
	
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

	DebugHelper::AddMessageToLog("Chance extracted: " + FString::SanitizeFloat(Chance));

	for (const auto& Entry : Filtered)
	{
		WeightAccumulator += Entry.Value / TotalWeight;
		if (Chance <= WeightAccumulator)
		{
			LastDecision = Entry.Key;
			DebugHelper::AddMessageToLog("Decision picked: " + GetDecisionString(Entry.Key));
			return Entry.Key;
		}
	}

	return EDecision::None;
}


FString FDecisionMaker::GetDecisionString(const EDecision& Decision) const
{
	switch (Decision)
	{
	case EDecision::HealItSelf:
		return "Heal";
	case EDecision::HealOther:
		return "Heal Other";
	case EDecision::FreezedUp:
		return "FreezedUp";
	case EDecision::BuffItSelf:
		return "Buff Atk";
	case EDecision::BuffOther:
		return "Buff Other Atk";
	case EDecision::DebuffDefence:
		return "Debuff Def";
	case EDecision::BuffDefence:
		return "Buff Def";
	case EDecision::BuffOtherDefence:
		return "Buff Other Def";
	case EDecision::DebuffAtk:
		return "Debuff Atk";
	case EDecision::DebuffShieldItSelf:
		return "Debuff Shield";
	case EDecision::DebuffShieldOther:
		return "Debuff Other Shield";
	case EDecision::EnvyBurned:
		return "Envy Burned";
	case EDecision::None:
		return "Attack";
	default:
	case EDecision::Invalid:
		return "";
	}
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

