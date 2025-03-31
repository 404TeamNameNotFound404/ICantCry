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

	
}

void FTurn::AssignFirstTurn()
{
	if (Queue.IsEmpty())
	{
		DebugHelper::LogError("Queue is empty can't assign first turn");
		return;
	}
	
	const int Aleatory =  FMath::RandRange(0, Queue.Num() - 1);
	CurrentTurn = Aleatory;
	NextTurn = (Aleatory + 1) % (Queue.Num());

	DebugHelper::LogSuccess(Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::LogMessage(3, FColor::Blue, Queue[NextTurn]->GetName() + " will play next");
}
