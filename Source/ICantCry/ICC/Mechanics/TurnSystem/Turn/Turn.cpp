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
		DebugHelper::AddMessageToLog(It->GetActorLabel() + " joined the fun");
	}

	
}

void FTurn::RejoinQueue(const TArray<AMob*>& Emotions)
{
	for (AMob* Mob : Emotions)
	{
		AICC_Actor* AsActor = Cast<AICC_Actor>(Mob);

		if (Queue.Contains(AsActor))
		{
			continue;
		}

		Queue.Emplace(AsActor);
		DebugHelper::LogMessage(10, FColor::Orange, Mob->GetActorLabel() + " rejoined the fun");
		DebugHelper::AddMessageToLog(Mob->GetActorLabel() + " rejoined the fun");
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
	DebugHelper::AddMessageToLog(Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::LogMessage(3, FColor::Blue, Queue[NextTurn]->GetName() + " will play next");
	DebugHelper::AddMessageToLog(Queue[NextTurn]->GetName() + " will play next");
}

void FTurn::AssignFirstTurnByPriority()
{
	if (Queue.IsEmpty())
	{
		DebugHelper::LogError("Queue is empty can't assign first turn");
		return;
	}

	Queue.Sort([](const AICC_Actor& A, const AICC_Actor& B)
	{
		if (A.GetSpeed() == B.GetSpeed())
		{
			return FMath::RandBool();
		}
		return A.GetSpeed() > B.GetSpeed();
	});


	CurrentTurn = 0;
	NextTurn = (CurrentTurn + 1) % Queue.Num();

	DebugHelper::LogSuccess(Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::AddMessageToLog(Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::LogMessage(3, FColor::Blue, Queue[NextTurn]->GetName() + " will play next");
	DebugHelper::AddMessageToLog(Queue[NextTurn]->GetName() + " will play next");
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

bool FTurn::CantBuffOthers()
{
	if (Queue.IsEmpty())
	{
		return false;
	}

	int32 MobCount = 0;

	for (AICC_Actor* Actor : Queue)
	{
		if (Actor->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}
		
		if (Actor->IsA(AMob::StaticClass()))
		{
			++MobCount;
		}
	}

	return MobCount <= 1;
}
