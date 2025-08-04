#include "DecisionMaker.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

FDecisionMaker::FDecisionMaker()
{
	FMath::RandInit(FPlatformTime::Cycles());
}

EDecision FDecisionMaker::Thought()
{
	const float Chance = FMath::FRand(); // Random float between 0.0 and 1.0
	float Counter = 0.0f;
	float TotalWeight = 0.0f;

	DebugHelper::LogMessage(6, FColor::Purple, "Chance extracted: " + FString::SanitizeFloat(Chance));

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
		TotalWeight += Entry.Value;
	}

	if (TotalWeight <= 0.0f)
	{
		DebugHelper::LogWarning("Total weight is zero or negative. Returning default decision.");
		return EDecision::None;
	}
	
	for (const auto& Entry : FinalMap)
	{
		const float NormalizedWeight = Entry.Value / TotalWeight;
		Counter += NormalizedWeight;
		
		DebugHelper::LogMessage(6, FColor::Magenta, "Decision " + UEnum::GetValueAsString(Entry.Key) + "| Normalized weight: " + FString::SanitizeFloat(NormalizedWeight) + " | Counter: " + FString::SanitizeFloat(Counter));

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

void FDecisionMaker::ResetDecision()
{
	LastDecision = EDecision::Invalid;
}

bool FDecisionMaker::CanRepeat(const EDecision& Decision)
{
	 return Decision ==  EDecision::None;
}

