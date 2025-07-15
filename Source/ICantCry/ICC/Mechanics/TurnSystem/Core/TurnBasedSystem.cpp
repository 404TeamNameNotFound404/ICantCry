#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "EngineUtils.h"
#include "FunctionalUIScreenshotTest.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/BattleHandler.h"
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

	for (TActorIterator<ABattleHandler> It(World); It; ++It)
	{
		BattleHandler = *It;
		break;
	}
	
	bFightStarted = true;

	FTimerHandle DelayHandle;
	World->GetTimerManager().SetTimer(DelayHandle, [this, World]()
	{
		if (EnemySpawnManager)
		{
			EnemySpawnManager->SpawnRandomEnemy();
			Turn.PopulateQueue(World);
		}
	}, 0.5f, false);
	
	CurrentPlayer->GetBattleHUD()->ShowHUD();
	
	DebugHelper::LogSuccess("Fight started right after");
}

void UTurnBasedSystem::Update(UWorld* World)
{
	if (!bRequestFight)
	{
		return;
	}
	
	if (!World)
	{
		DebugHelper::LogError("World is null! at UTurnBasedSystem::Update");
		return;
	}

	if (!bInit)
	{
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
				CurrentPlayer->GetBattleHUD()->SetCurrentPlayingEmotion(Mob);
				Mob->PlayTurn();
			}
			//otherwise is player playing
			else
			{
				bIsAiTurn = false;
				bIsPlayerTurn = true;
				AICC_Player* Player = Cast<AICC_Player>(Who);
				DebugHelper::AddTurnMaterialOverlayToStaticMesh(Player->DebugMesh);
				CurrentPlayer = Player;
				BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Your Turn"));
			}
		}

		bInit = true;
	}
	
	if (bIsAiTurn)
	{
		if (!bAIPlayTurn)
		{
			bAIPlayTurn = true; 
			AMob* Mob = Cast<AMob>(Turn.Queue[Turn.CurrentTurn]);
			checkf(Mob, TEXT("Mob invalid at UTurnBasedSystem::Update"))
			DebugHelper::LogWarning(Mob->GetActorLabel() + " Turn");
			AICC_AIController* AIController = Cast<AICC_AIController>(Mob->GetController());
			//BattleHandler->GetBattleInfo()->SetTurnInfo(FText::FromString(Mob->GetActorLabel() + " Turn\n AI Controller id : " + AIController->GetActorLabel()));
			//Mob->GetBattleHandler()->GetBattleInfo()->SetTurnInfo(FText::FromString(Mob->GetActorLabel() + " Turn\n AI Controller id : " + AIController->GetActorLabel()));
			CurrentPlayer->GetBattleHUD()->SetCurrentPlayingEmotion(Mob);
			Mob->PlayTurn();
			//Mob->PlaySecondTurn();
		}

		Turn.Timer += World->GetDeltaSeconds() * Variations;
	}

	if (bIsPlayerTurn && CurrentPlayer)
	{
		DebugHelper::AddTurnMaterialOverlayToStaticMesh(CurrentPlayer->DebugMesh);
		BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Your Turn"));
		
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
	if (bAIPlayTurn)
	{
		DebugHelper::LogError("AI Turn Ended");
		bAIPlayTurn = false;
	}

	Turn.CurrentTurn = Turn.NextTurn;
	Turn.NextTurn = (Turn.NextTurn + 1) % Turn.Queue.Num();
	DebugHelper::LogWarning("turn ended, " + Turn.Queue[Turn.CurrentTurn]->GetName() + " will now play");
}

FTurn UTurnBasedSystem::GetTurn() const
{
    return Turn;
}

bool UTurnBasedSystem::GetIsPlayerTurn() const
{
	return bIsPlayerTurn;
}

bool UTurnBasedSystem::GetIsAITurn() const
{
	return bIsAiTurn;
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
	if (!CurrentPlayer->GetBattleHUD()->IsReadyToBattle())
	{
		return;
	}

	for (int32 i = Turn.Queue.Num() - 1; i >= 0; --i) 
	{
		AICC_Actor* Actor = Turn.Queue[i];
		AMob* Mob = Cast<AMob>(Actor);

		if (Mob && !Mob->IsAlive())
		{
			Turn.Queue.RemoveAt(i);
			DebugHelper::LogWarning("Mob removed from queue due to death.");
		}
	}

	for (int32 i = Turn.GetEmotionsInBattle().Num() - 1; i >= 0; --i)
	{
		AICC_Actor* Actor = Turn.Queue[i];
		AMob* Mob = Cast<AMob>(Actor);

		if (Mob && !Mob->IsAlive())
		{
			Turn.GetEmotionsInBattle().RemoveAt(i);
		}
	}

	if (Turn.Queue.Num() == 1 && Turn.Queue[0] == CurrentPlayer)
	{

		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;
		
		BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Victory!"));
		
		//TODO ADD VICTORY SCREEN
		return;
	}


	if (!CurrentPlayer->IsAlive()) 
	{

		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;
		
		BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Game Over"));

		//TODO ADD GAMEOVER SCREEN
	}
	
}


void UTurnBasedSystem::RequestFight(const bool& Request)
{
	bRequestFight = Request;
}

void UTurnBasedSystem::SetAIPlaying(const bool& Play)
{
	bAIPlayTurn = Play;
}

void UTurnBasedSystem::ExitBattle()
{
	BattleHandler->GetBattleInfo()->RemoveFromParent();
	CurrentPlayer->GetInGameMenu()->SetDisabled(false);
	bRequestFight = false;
	bInit = false;
}

