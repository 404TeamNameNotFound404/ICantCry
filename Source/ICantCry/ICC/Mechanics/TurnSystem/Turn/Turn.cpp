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
		DebugHelper::AddMessageToLog("[Turn]: " + It->GetName() + " joined the fun");
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
		DebugHelper::AddMessageToLog("[Turn]: " + Mob->GetEmotionName() + " rejoined the fun");
	}
	
}

void FTurn::RejoinQueue(const TArray<AMob*>& Emotions, AICC_Player* Player)
{
	Queue.Reset();

	Queue.Add(Player);
	
	for (AMob* Mob : Emotions)
	{
		if (!Mob) continue;

		AICC_Actor* AsActor = Cast<AICC_Actor>(Mob);
		if (!AsActor) continue;

		Queue.Add(AsActor);

		DebugHelper::AddMessageToLog(
			"[Turn]: " + Mob->GetEmotionName() + " rejoined the fun"
		);
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
	DebugHelper::AddMessageToLog("[Turn]: " + Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::AddMessageToLog("[Turn]: " + Queue[NextTurn]->GetName() + " will play next");
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
	
	DebugHelper::AddMessageToLog("[Turn]: " + Queue[CurrentTurn]->GetName() + " will start");
	DebugHelper::AddMessageToLog("[Turn]: " + Queue[NextTurn]->GetName() + " will play next");
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

AMob* FTurn::GetMobInQueue(AMob* Emotion) const
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
			if (Mob == Emotion) continue;
			
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

bool FTurn::LastMobStanding()
{
	int32 MobCount = 0;
	
	if (Queue.IsEmpty()) return false;

	for (const AICC_Actor* Actor : Queue)
	{
		if (Actor->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}
		
		MobCount++;
	}
	
	return MobCount == 1;
}

bool FTurn::AreAnxietyOrCalmAlone()
{
	if (Queue.IsEmpty())
	{
		return false;
	}

	int32 MobCount = 0;

	for (const AICC_Actor* Actor : Queue)
	{
		if (Actor->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		const AMob* Mob = Cast<AMob>(Actor);

		if (!Mob)
		{
			continue;
		}

		MobCount++;

		if (Mob->GetMobType() != EMobType::MobAnxiety &&
			Mob->GetMobType() != EMobType::MobCalm)
		{
			return false;
		}
	}

	return MobCount > 0;
}
