#include "Turn.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
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

		DebugHelper::LogMessage(10, FColor::Black, It->GetActorLabel() + " joined the fun");
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

AMob* FTurn::GetMobInQueue() const
{
	if (Queue.IsEmpty())
	{
		return nullptr;
	}

	const int32 MaxTries = Queue.Num();
	for (int32 i = 0; i < MaxTries; ++i)
	{
		const int32 Aleatory = FMath::RandRange(0, Queue.Num() - 1);
		if (AMob* Mob = Cast<AMob>(Queue[Aleatory]))
		{
			return Mob;
		}
	}

	return nullptr;
}
