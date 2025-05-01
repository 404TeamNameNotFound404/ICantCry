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

	DebugHelper::LogWarning("Chance is " + FString::SanitizeFloat(Chance));
	
	for (auto& Entry : DecisionMap)
	{
		Counter += Entry.Value;

		if (Chance < Counter)
		{
			DebugHelper::LogError("Checking " + FString(*UEnum::GetValueAsString(Entry.Key) + FString(" Counter ") + FString::SanitizeFloat(Counter)));
			return Entry.Key;
		}
	}

	return EDecision::None;
}

