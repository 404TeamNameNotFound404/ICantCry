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
			TryGetCurrentPlayer()->GetBattleHUD()->SpawnVisualizer();
			TryGetCurrentPlayer()->GetBattleHUD()->SpawnGameOverVisualizer();
			CopyQueue = Turn.Queue;
		}
	}, 0.5f, false);
	
	
	CurrentPlayer->GetBattleHUD()->ShowHUD();
	DebugHelper::LogSuccess("Fight started right after");
	
}

void UTurnBasedSystem::Start2(UWorld* World, FBattleMemory* Memory)
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

	Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	checkf(Instance, TEXT("Instance is null at start2"))
	
	FTimerHandle DelayHandle;
	World->GetTimerManager().SetTimer(DelayHandle, [this, World, Memory]()
	{
		EnemySpawnManager->SpawnRandomEnemy();
		Turn.PopulateQueue(World);
		Memory->Load(Turn.Queue, Instance->GetInventory().BulletsStored);
		TryGetCurrentPlayer()->GetBattleHUD()->SpawnVisualizer();
		TryGetCurrentPlayer()->GetBattleHUD()->SpawnGameOverVisualizer();
	}, 0.5f, false);
	

	FTimerHandle DelayHudHandle;
	World->GetTimerManager().SetTimer(DelayHudHandle, [this]()
	{
		CurrentPlayer->GetBattleHUD()->ShowHUD();
	}, 5.f, false);
	
	// CurrentPlayer->GetBattleHUD()->ShowHUD();
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
				
				if (Mob->IsAIReadyToPlay())
				{
					Mob->PlayTurn();
				}
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
			CurrentPlayer->GetBattleHUD()->SetCurrentPlayingEmotion(Mob);
			
			if (Mob->IsAIReadyToPlay())
			{
				Mob->PlayTurn();
			}
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
		Instance->GetCurrentPlayer()->GetBattleHUD()->SetApAccumulator(0);
	}

	Turn.CurrentTurn = Turn.NextTurn;
	Turn.NextTurn = (Turn.NextTurn + 1) % Turn.Queue.Num();
}

FTurn& UTurnBasedSystem::GetTurn()
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

	if (Turn.Queue.Num() == 1 && Turn.Queue[0] == CurrentPlayer)
	{

		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;
		
		BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Victory!"));

		if (!bVictory)
		{
			TryGetCurrentPlayer()->GetBattleHUD()->DisplayVictoryVisualizer();
			TryGetCurrentPlayer()->GetBattleHUD()->GetVictoryVisualizer()->AfterBattle(EnemySpawnManager->GetMemory().LastStoredQueue);
			bVictory = true;
		}
		
		return;
	}


	if (!CurrentPlayer->IsAlive()) 
	{
		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;
		BattleHandler->GetBattleInfo()->SetInfo(FText::FromString("Game Over"));
		
		TryGetCurrentPlayer()->GetBattleHUD()->DisplayGameOverVisualizer();
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

void UTurnBasedSystem::SpawnBattleVictory(UWorld* World)
{
}

void UTurnBasedSystem::ExitBattle()
{
	BattleHandler->GetBattleInfo()->RemoveFromParent();
	CurrentPlayer->GetInGameMenu()->SetDisabled(false);
	bRequestFight = false;
	bInit = false;
	bVictory = false;
	Turn.Queue.Empty();
	EnemySpawnManager->GetMemory().Clear();
}

void UTurnBasedSystem::Reload()
{
	bRequestFight = true;
	bInit = false;
	bVictory = false;
	Instance->GetInventory().BulletsStored = EnemySpawnManager->GetMemory().InBattleBullets;
	Instance->GetCurrentPlayer()->GetBattleHUD()->SetBulletSetupFinished(false);
	Turn.CurrentTurn = 0;
}


TArray<AICC_Actor*> UTurnBasedSystem::GetCopyQueue() const
{
	return CopyQueue;
}
