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
	
	for (auto& Entry : DecisionMap)
	{
		Counter += Entry.Value;

		if (Chance < Counter)
		{
			return Entry.Key;
		}
	}

	return EDecision::None;
}

void FDecisionMaker::Clear()
{
	DecisionMap.Empty();
}

