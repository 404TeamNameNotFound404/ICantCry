#include "Turn.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

FTurn::FTurn() : Timer(0), CurrentTurn(-1), NextTurn(-1)
{
	Queue.Empty();
}

void FTurn::PopulateQueue(UWorld* World)
{
	if (!World)
	{
		DebugHelper::LogError("World is null");
		return;
	}

	for (TActorIterator<AICC_Actor> It(World); It; ++It)
	{
		if (!IsValid(*It))
		{
			DebugHelper::LogError("Invalid actor found, skipping...");
			continue;
		}
		
		Queue.Add(*It);
	}
	
	DebugHelper::LogSuccess("Populate queue " + FString::FromInt(Queue.Num()));
}
