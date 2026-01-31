#include "DecisionMaker.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/AI/BehaviorNodes/Default/UBTTask_DefaultAtk.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"

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
    	DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Buffed Atk");
    }
    else if (Current->GetStatusTracker()->GetPerkData().bDebuffAtk)
    {
	    CurrentWeights = Current->GetDecisionTable()->DebuffAtk;
    	DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Debuff Atk");
    }
    else if (Current->IsLowHealth())
    {
    	Current->GetStatusTracker()->GetPerkData().bBuffAtk = false;
    	Current->GetStatusTracker()->GetPerkData().bDebuffAtk = false;
    	Current->GetStatusTracker()->GetPerkData().bBuffDef = false;
    	Current->GetStatusTracker()->GetPerkData().bDebuffDef = false;
	    CurrentWeights = Current->GetDecisionTable()->LowHealth;
    	DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Low health");
    }
    else if (Current->GetStatusTracker()->GetPerkData().bEnvyBurned)
    {
	    CurrentWeights = Current->GetDecisionTable()->EnvyBurned;
    	DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Envy burned");
    }
	else if (Current->GetStatusTracker()->GetPerkData().bBuffDef)
	{
		CurrentWeights = Current->GetDecisionTable()->BuffDef;
		DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Buff Def");
	}
	else if (Current->GetStatusTracker()->GetPerkData().bDebuffDef)
	{
		CurrentWeights = Current->GetDecisionTable()->DebuffDef;
		DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Debuff Def");
	}
	else if (Current->GetStatusTracker()->GetPerkData().bShieldDebuff)
	{
		CurrentWeights = Current->GetDecisionTable()->DebuffShield;
		DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Shield");
	}
	else if (Current->GetStatusTracker()->GetPerkData().bAshamed)
	{
		CurrentWeights = Current->GetDecisionTable()->Ashamed;
		DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Ashamed");
	}
    else
    {
	    CurrentWeights = Current->GetDecisionTable()->Idle;
    	DebugHelper::AddMessageToLog("[Decision Maker]: Decision table used: Idle");
    }
	
	UStatusTracker* StatusTracker = Current->GetStatusTracker();
	
	switch (Current->GetMobType())
	{
	case MobAnger:
		if (CurrentWeights.BuffAtkWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::BuffItSelf, CurrentWeights.BuffAtkWeight);
		
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobShame:
		if (CurrentWeights.AttackWeight > 0.f)
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		break;
    case MobJoy:
		if (CurrentWeights.HealWeight > 0.f)
			DecisionMap.Add(EDecision::HealItSelf, CurrentWeights.HealWeight);
		
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		
		if (CurrentWeights.HealOtherWeight > 0.f)
			DecisionMap.Add(EDecision::HealOther, CurrentWeights.HealOtherWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobDisgust:
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		
		if (CurrentWeights.DebuffAtkWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::DebuffAtk, CurrentWeights.DebuffAtkWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobFear:
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		
		if (CurrentWeights.BuffDefWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::BuffDefence, CurrentWeights.BuffDefWeight);
		
		if (CurrentWeights.BuffOtherDefWeight > 0.f )
			DecisionMap.Add(EDecision::BuffOtherDefence, CurrentWeights.BuffOtherDefWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobJealousy:
		if (CurrentWeights.EnvyBurnedWeight > 0.f)
			DecisionMap.Add(EDecision::EnvyBurned, CurrentWeights.EnvyBurnedWeight);
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		if (CurrentWeights.BuffOtherAtkWeight > 0.f)
			DecisionMap.Add(EDecision::BuffOther, CurrentWeights.BuffOtherAtkWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobSadness:
		if (CurrentWeights.AttackWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::None, CurrentWeights.AttackWeight);
		
		if (CurrentWeights.DebuffDefWeight > 0.f && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::DebuffDefence, CurrentWeights.DebuffDefWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
	case MobAnxiety:
		if (CurrentWeights.FreezedUpWeight > 0.f)
			DecisionMap.Add(EDecision::FreezedUp, CurrentWeights.FreezedUpWeight);
		
		if (CurrentWeights.DebuffDefWeight > 0.f)
			DecisionMap.Add(EDecision::DebuffDefence, CurrentWeights.DebuffDefWeight);
		
		if (CurrentWeights.DebuffAtkWeight > 0.f)
			DecisionMap.Add(EDecision::DebuffAtk, CurrentWeights.DebuffAtkWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
 		break;
	case MobCalm:
		if (CurrentWeights.ShieldWeight > 0.f )
			DecisionMap.Add(EDecision::DebuffShieldItSelf, CurrentWeights.ShieldWeight);
		
		if (CurrentWeights.ShieldOtherWeight > 0.f )
			DecisionMap.Add(EDecision::DebuffShieldOther, CurrentWeights.ShieldOtherWeight);
		
		if (CurrentWeights.BuffDefWeight > 0.f  && !Current->IsLowHealth())
			DecisionMap.Add(EDecision::BuffDefence, CurrentWeights.BuffDefWeight);
		
		if (CurrentWeights.BuffOtherDefWeight > 0.f)
			DecisionMap.Add(EDecision::BuffOtherDefence, CurrentWeights.BuffOtherDefWeight);
		
		if (CurrentWeights.LowHealthWeight > 0.f)
			DecisionMap.Add(EDecision::Low, CurrentWeights.LowHealthWeight);
		break;
    default:
    	break;
	}
}

EDecision FDecisionMaker::Thought()
{
	DebugHelper::AddMessageToLog("[Decision Maker]: AI is thinking the best move..");
	
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

	DebugHelper::AddMessageToLog("[Decision Maker]: Chance extracted: " + FString::SanitizeFloat(Chance));

	for (const auto& Entry : Filtered)
	{
		WeightAccumulator += Entry.Value / TotalWeight;
		if (Chance <= WeightAccumulator)
		{
			LastDecision = Entry.Key;
			DebugHelper::AddMessageToLog("[Decision Maker]: Decision picked: " + GetDecisionString(Entry.Key));
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
	case EDecision::Low:
		return "LowHealth";
	default:
	case EDecision::Invalid:
		return "";
	}
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

