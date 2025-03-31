#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

UTurnBasedSystem::UTurnBasedSystem() : MaxAITurnTime(10.0f), bIsAiTurn(false), bIsPlayerTurn(false),
                                       CurrentPlayer(nullptr)
{
	Turn.Timer = 0.0f;
	Turn.CurrentTurn = -1;
	Turn.NextTurn = -1;
	Turn.Queue.Empty();
}

void UTurnBasedSystem::Start(UWorld* World)
{

	for (TActorIterator<AEnemySpawnManager> It(World); It; ++It)
	{
		EnemySpawnManager = *It;
		DebugHelper::LogSuccess("EnemySpawnManager FOUND");
		break;

	}

	EnemySpawnManager->SpawnRandomEnemy();

	Turn.PopulateQueue(World);
	Turn.AssignFirstTurn();
	
	if (Turn.Queue.IsValidIndex(Turn.CurrentTurn))
	{
		AICC_Actor* Who = Turn.Queue[Turn.CurrentTurn];
		// if first to play is Emotion / AI
		if (Who->IsA(AMob::StaticClass()))
		{
			AMob* Mob = Cast<AMob>(Who);
			bIsAiTurn = true;
			bIsPlayerTurn = false;
			// Mob->PlayTurn(); //TODO ADD MOB PLAY TURN (Define the ai class)
		}
		//otherwise is player playing
		else
		{
			bIsAiTurn = false;
			bIsPlayerTurn = true;
			AICC_Player* Player = Cast<AICC_Player>(Who);
			CurrentPlayer = Player;
			// Player->PlayTurn(); // TODO ADD PLAYER PLAY TURN
		}
	}
}

void UTurnBasedSystem::Update(UWorld* World)
{
	if (!World)
	{
		DebugHelper::LogError("World is null! at UTurnBasedSystem::Update");
		return;
	}
	
	if (bIsAiTurn)
	{
		Turn.Timer += World->GetDeltaSeconds() * Variations;
		
		if (Turn.Timer >= MaxAITurnTime)
		{
			Turn.Timer = 0;
			DebugHelper::LogError(Turn.Queue[Turn.CurrentTurn]->GetName() + " ended it's turn");
			EndTurn();
			StartNextTurn();
		}
	}

	if (bIsPlayerTurn)
	{
		// Waiting for the actual implementation for now the player will skip the turn
		Turn.Timer += World->GetDeltaSeconds() * Variations;
		if (Turn.Timer >= MaxAITurnTime)
		{
			Turn.Timer = 0;
			EndTurn();
			StartNextTurn();
		}
		// the code above must be deleted once the real implementation is done
		/* CODE PROTOTYPE BELOW
		 *
		 * Player->PlayTurn();
		 * 
		 * if(Player->TurnDone()
		 * {
		 *    EndTurn();
		 *    StartNextTurn();
		 * } */
	}
	
	Flow();
}

void UTurnBasedSystem::StartNextTurn()
{
	DebugHelper::LogWarning("Next turn is started");
	
	if (Turn.Queue.IsValidIndex(Turn.CurrentTurn))
	{
		AICC_Actor* Who = Turn.Queue[Turn.CurrentTurn];
		// if first to play is Emotion / AI
		if (Who->IsA(AMob::StaticClass()))
		{
			AMob* Mob = Cast<AMob>(Who);
			bIsAiTurn = true;
			bIsPlayerTurn = false;
			// Mob->PlayTurn(); //TODO ADD MOB PLAY TURN (Define the ai class)
		}
		//otherwise is player playing
		else
		{
			bIsAiTurn = false;
			bIsPlayerTurn = true;
			AICC_Player* Player = Cast<AICC_Player>(Who);
			CurrentPlayer = Player;
			// Player->PlayTurn(); // TODO ADD PLAYER PLAY TURN
		}
	}
}

void UTurnBasedSystem::EndTurn()
{
	Turn.CurrentTurn = Turn.NextTurn;
	Turn.NextTurn = (Turn.NextTurn + 1) % Turn.Queue.Num();
	DebugHelper::LogWarning("Ai turn ended, " + Turn.Queue[Turn.CurrentTurn]->GetName() + " will now play");
}

void UTurnBasedSystem::Flow()
{
	/*
	* In case of AI death
	* Remove its index from the array and if array is empty victory condition is achieved
	*/

	if (Turn.Queue.Num() <= 1 && Turn.Queue[0] == CurrentPlayer) // 
	{
		/*
		 * Victory
		 * TODO IMPLEMENT VICTORY
		 */
	}
/*
	else if (CurrentPlayer->Death())
	{
		GameOver!
		TODO ADD GAME OVER
	}
*/
}




