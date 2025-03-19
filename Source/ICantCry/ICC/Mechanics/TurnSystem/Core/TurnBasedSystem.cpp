#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

UTurnBasedSystem::UTurnBasedSystem() : Variations(2.0f) , MaxAITurnTime(10.0f), bIsAiTurn(false), bIsPlayerTurn(false)
{
	Turn.Timer = 0.0f;
	Turn.CurrentTurn = -1;
	Turn.NextTurn = -1;
	Turn.Queue.Empty();

	DebugHelper::LogSuccess(FString::FromInt(Turn.CurrentTurn));
}

void UTurnBasedSystem::Start(UWorld* World)
{
	PopulateQueue(World);
	AssignFirstTurn();
	if (Turn.Queue.IsValidIndex(Turn.CurrentTurn))
	{
		AICC_Actor* Who = Turn.Queue[Turn.CurrentTurn];
		// if first to play is Emotion / AI
		if (Who->IsA(AMob::StaticClass()))
		{
			AMob* Mob = Cast<AMob>(Who);
			bIsAiTurn = true;
			bIsPlayerTurn = false;
			// Mob->PlayTurn(); //TODO ADD MOB PLAY TURN
		}
		//otherwise is player playing
		else
		{
			bIsAiTurn = false;
			bIsPlayerTurn = true;
			AICC_Player* Player = Cast<AICC_Player>(Who);
			// Player->PlayTurn(); // TODO ADD PLAYER PLAY TURN
		}
	}
}

void UTurnBasedSystem::Update(UWorld* World)
{
	if (bIsAiTurn)
	{
		Turn.Timer += GetWorld()->GetDeltaSeconds() * Variations;
		if (Turn.Timer >= MaxAITurnTime)
		{
			Turn.Timer = 0;
			End();
			Start(World);
		}
	}

	if (bIsPlayerTurn)
	{
		/* if(Player->TurnDone()
		 * {
		 *    End();
		 *    Start();
		 * } */
	}
}

void UTurnBasedSystem::End()
{
	Turn.CurrentTurn = Turn.NextTurn;
	Turn.NextTurn = (Turn.NextTurn + 1) % Turn.Queue.Num();
}

void UTurnBasedSystem::AssignFirstTurn()
{
	if (Turn.Queue.IsEmpty())
	{
		DebugHelper::LogError("Queue is empty can't assign first turn");
		return;
	}
	
	const int Aleatory =  FMath::RandRange(0, Turn.Queue.Num() - 1);
	Turn.CurrentTurn = Aleatory;
	Turn.NextTurn = (Aleatory + 1) % (Turn.Queue.Num());

	DebugHelper::LogSuccess(Turn.Queue[Turn.CurrentTurn]->GetName() + " will start");
	DebugHelper::LogMessage(3, FColor::Blue, Turn.Queue[Turn.NextTurn]->GetName() + " will play next");
	
}

void UTurnBasedSystem::PopulateQueue(UWorld* World)
{
	Turn.PopulateQueue(World);
}



