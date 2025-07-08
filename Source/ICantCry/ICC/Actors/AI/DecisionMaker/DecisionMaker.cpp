#include "DecisionMaker.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

FDecisionMaker::FDecisionMaker()
{
	FMath::RandInit(FPlatformTime::Cycles());
}

EDecision FDecisionMaker::Thought()
{
	const float Chance = FMath::FRand();
	float Counter = 0.0f;
	
	TMap<EDecision, float> FilteredMap;

	for (const auto& Entry : DecisionMap)
	{
		if (Entry.Key != LastDecision || CanRepeat(Entry.Key))
		{
			FilteredMap.Add(Entry.Key, Entry.Value);
		}
	}
	
	const TMap<EDecision, float>& FinalMap = FilteredMap.Num() > 0 ? FilteredMap : DecisionMap;

	for (const auto& Entry : FinalMap)
	{
		Counter += Entry.Value;
		if (Chance < Counter)
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

bool FDecisionMaker::CanRepeat(const EDecision& Decision)
{
	 return Decision ==  EDecision::None;
}

