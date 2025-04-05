#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "EngineUtils.h"
#include "FunctionalUIScreenshotTest.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"

UTurnBasedSystem::UTurnBasedSystem() : MaxAITurnTime(10.0f), bIsAiTurn(false), bIsPlayerTurn(false),
                                       CurrentPlayer(nullptr)
{
	Turn.Timer = 0.0f;
	Turn.CurrentTurn = -1;
	Turn.NextTurn = -1;
	Turn.Queue.Empty();
}

static AMob* CurrentMob = nullptr;

void UTurnBasedSystem::Start(UWorld* World)
{
	for (TActorIterator<AEnemySpawnManager> It(World); It; ++It)
	{
		EnemySpawnManager = *It;
		DebugHelper::LogSuccess("EnemySpawnManager FOUND");
		break;
	}

	for (TActorIterator<AICC_Player> It(World); It; ++It)
	{
		CurrentPlayer = *It;
		break;
	}
	
	//EnemySpawnManager->SpawnRandomEnemy();

	Turn.PopulateQueue(World);
	Turn.AssignFirstTurn();
	CurrentPlayer->GetBattleHUD()->ShowHUD();
	
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
		AMob* Mob = Cast<AMob>(Turn.Queue[Turn.CurrentTurn]);
		Mob->HighlightsSilhouette();
		
		if (Turn.Timer >= MaxAITurnTime)
		{
			Turn.Timer = 0;
			DebugHelper::LogError(Turn.Queue[Turn.CurrentTurn]->GetName() + " ended it's turn");
			CurrentPlayer->GetMinigameHandler()->StartMinigame(false);
			Mob->DisableSilhouette();
			EndTurn();
			StartNextTurn();
		}
	}

	if (bIsPlayerTurn && CurrentPlayer)
	{
		if (CurrentPlayer->GetBattleHUD()->IsShootFired())
		{
			if (!CurrentPlayer->GetBattleHUD()->CanvasStatus->IsVisible())
			{
				DebugHelper::LogWarning("Canvas status is already visible");
				CurrentPlayer->GetBattleHUD()->CanvasStatus->SetVisibility(ESlateVisibility::Visible);
			}
			
			if (Turn.Queue.Num() > 0)
			{
				CurrentPlayer->GetBattleHUD()->UpdateTarget();
			}
		}
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
			CurrentMob = Mob;
			bIsAiTurn = true;
			bIsPlayerTurn = false;
			// Mob->PlayTurn(); //TODO ADD MOB PLAY TURN (Define the ai class)
		}
		//otherwise is player playing
		else
		{
			DebugHelper::LogMessage(3, FColor::FromHex("4D55CC"), "Player turn!");
			bIsAiTurn = false;
			bIsPlayerTurn = true;
			AICC_Player* Player = Cast<AICC_Player>(Who);
			
			if (!bTurnOverlayApplied)
			{
				DebugHelper::AddTurnMaterialOverlayToStaticMesh(Player->DebugMesh);
				bTurnOverlayApplied = true;
			}
			
			CurrentPlayer = Player;
		}
	}
}

void UTurnBasedSystem::EndTurn()
{
	Turn.CurrentTurn = Turn.NextTurn;
	Turn.NextTurn = (Turn.NextTurn + 1) % Turn.Queue.Num();
	DebugHelper::LogWarning("Ai turn ended, " + Turn.Queue[Turn.CurrentTurn]->GetName() + " will now play");
}

FTurn UTurnBasedSystem::GetTurn() const
{
    return Turn;
}

bool UTurnBasedSystem::GetIsPlayerTurn() const
{
	return bIsPlayerTurn;
}

AICC_Player* UTurnBasedSystem::TryGetCurrentPlayer() const
{
	if (!CurrentPlayer)
	{
		return nullptr;
	}
	
	return CurrentPlayer;
}

void UTurnBasedSystem::SetTurnOverlayApplied(const bool& Applied)
{
	bTurnOverlayApplied = Applied;
}

void UTurnBasedSystem::Flow()
{
	/* TODO ADD A WAY TO SPEED UP THE BATTLE USING BATTLE FLOW IN PLAYER STATS
	* In case of AI death
	* Remove its index from the array and if array is empty victory condition is achieved
	*/

	if (Turn.Queue.Num() <= 1 && Turn.Queue[0] == CurrentPlayer) // 
	{
		/*
		 * Victory
		 * TODO IMPLEMENT VICTORY AND GAIN EXPERIENCE
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




