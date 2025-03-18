#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

UTurnBasedSystem::UTurnBasedSystem() : Variations(2.0f) , MaxAITurnTime(10.0f), bIsAiTurn(false), bIsPlayerTurn(false)
{
	
}

void UTurnBasedSystem::Start()
{
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

void UTurnBasedSystem::Update()
{
	if (bIsAiTurn)
	{
		Turn.Timer += GetWorld()->GetDeltaSeconds() * Variations;
		if (Turn.Timer >= MaxAITurnTime)
		{
			Turn.Timer = 0;
			End();
			Start();
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
	
	const int Aleatory =  FMath::RandRange(0, Turn.Queue.Num());
	Turn.CurrentTurn = Aleatory;
	Turn.NextTurn = (Aleatory + 1) % (Turn.Queue.Num() + 1);

	DebugHelper::LogMessage(5, FColor::White, "Current Turn " + FString::FromInt(Turn.CurrentTurn));
	DebugHelper::LogWarning("Next Turn " + FString::FromInt(Turn.NextTurn));
}



