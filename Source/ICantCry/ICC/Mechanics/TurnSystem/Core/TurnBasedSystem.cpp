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
}

static TArray<AICC_Actor*> StaticQueue;

void UTurnBasedSystem::Start2(UWorld* World, FBattleMemory* Memory)
{
	for (TActorIterator<AEnemySpawnManager> It(World); It; ++It)
	{
		EnemySpawnManager = *It;
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
	
	Instance->GetPlayerStats()->RuntimeStats = Instance->GetRuntimeStats();
	
	FTimerHandle DelayHandle;
	World->GetTimerManager().SetTimer(DelayHandle, [this, World]()
	{
	   if (Instance->CachedBattleMemory.bBattleRetried)
	   {
		  TArray<AMob*> SpelledMobs = EnemySpawnManager->SpawnFixedEnemies();
      
		  Turn.Queue.Empty();
		  Turn.Queue.Add(Instance->GetCurrentPlayer());
	   	
		  for (const auto M : SpelledMobs)
		  {
			 Turn.Queue.Add(Cast<AICC_Actor>(M));
		  }
      
		  Instance->CachedBattleMemory.bBattleRetried = false;
	   	Reload();
	   }
	   else
	   {
		  EnemySpawnManager->SpawnRandomEnemy();
		  Turn.PopulateQueue(World);
	   }
   
	   StaticQueue = Turn.Queue;
	   Instance->CachedBattleMemory.LastStoredQueue = StaticQueue;
	   TryGetCurrentPlayer()->GetBattleHUD()->SpawnVisualizer();
	   TryGetCurrentPlayer()->GetBattleHUD()->SpawnGameOverVisualizer();
	}, 0.5f, false);
	
	FTimerHandle DelayHudHandle;
	World->GetTimerManager().SetTimer(DelayHudHandle, [this]()
	{
		CurrentPlayer->GetBattleHUD()->ShowHUD();
	}, 5.f, false);
	
	DebugHelper::ClearAllLogs();
	DebugHelper::AddMessageToLog("-----Battle Log-----\n");
	DebugHelper::AddMessageToLog("[Turn System]: Fight started right after");
	SetBattlePhase(EBattlePhase::Preparation);
}

void UTurnBasedSystem::Update(UWorld* World, FBattleMemory* Memory)
{
	if (!bRequestFight || BattlePhase == EBattlePhase::Preparation)
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
		//Turn.AssignFirstTurn();
		Turn.AssignFirstTurnByPriority();
	
		if (Turn.Queue.IsValidIndex(Turn.CurrentTurn))
		{
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Hide();
			// if first to play is Emotion / AI
			if (AICC_Actor* Who = Turn.Queue[Turn.CurrentTurn]; Who->IsA(AMob::StaticClass()))
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
				Instance->GetCurrentPlayer()->GetBinder()->FocusOn(Instance->GetCurrentPlayer()->GetBattleHUD()->Shoot);
				DebugHelper::AddMessageToLog("[Turn System]: Turn: " +  FString::FromInt(BattleTurnCounter) + " Your turn");
				Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
				Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("Your Turn"));
			}
		}

		bInit = true;
		SetBattlePhase(EBattlePhase::Running);
	}
	
	if (bIsAiTurn)
	{
		if (!bAIPlayTurn)
		{
			bAIPlayTurn = true; 
			AMob* Mob = Cast<AMob>(Turn.Queue[Turn.CurrentTurn]);
			checkf(Mob, TEXT("Mob invalid at UTurnBasedSystem::Update"))
			DebugHelper::LogWarning(Mob->GetActorLabel() + " Turn");
			DebugHelper::AddMessageToLog("[Turn System]: Turn " + FString::FromInt(BattleTurnCounter) + " - " + Mob->GetActorLabel() + " Turn");
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
		DebugHelper::AddMessageToLog("[Turn System]: Your turn");
		
		if (!bHideDecision)
		{
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("Your Turn"));
			bHideDecision = true;
		}
		
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
	
	//Flow();
}

void UTurnBasedSystem::StartNextTurn()
{
	BattleTurnCounter++;
	
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
			DebugHelper::AddMessageToLog("[Turn System]: Turn " + FString::FromInt(BattleTurnCounter) + " - " + Mob->GetActorLabel() + "'s turn");
		}
		//otherwise is player playing
		else
		{
			bIsAiTurn = false;
			bIsPlayerTurn = true;
			AICC_Player* Player = Cast<AICC_Player>(Who);
			
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->Show();
			Instance->GetCurrentPlayer()->GetBattleHUD()->DecisionDisplayer->SetDecisionText(FText::FromString("Your Turn"));
			
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
		DebugHelper::AddMessageToLog("[Turn System]: AI Turn Ended");
		bAIPlayTurn = false;
		Instance->GetCurrentPlayer()->GetBattleHUD()->SetApAccumulator(0);
	}
	
	bHideDecision = false;
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
	DebugHelper::LogMessage(10, FColor::Orange, "Flow called");
	
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
			Mob->UnlockContentOnDeath();
			Turn.Queue.RemoveAt(i);
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetAPBar()->IncreaseAP(1);
			Instance->GetCurrentPlayer()->GetBattleHUD()->IncreaseAP(1);
			DebugHelper::LogWarning("Mob removed from queue due to death.");
			DebugHelper::AddMessageToLog("[Turn System]: " + Mob->GetActorLabel() + " died RIP.");
			Instance->GetCurrentPlayer()->GetBattleHUD()->ProcessExp(Mob);
		}
		
		else if (Mob && Mob->IsFleeing())
		{
			Turn.Queue.RemoveAt(i);
			Mob->Flee();
		}
	}

	if (Turn.Queue.Num() == 1 && Turn.Queue[0] == CurrentPlayer)
	{
		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;
		

		DebugHelper::AddMessageToLog("[Turn System]: Victory!");

		if (!bVictory)
		{
			TryGetCurrentPlayer()->GetBattleHUD()->DisplayVictoryVisualizer();
			TryGetCurrentPlayer()->GetBattleHUD()->GetVictoryVisualizer()->AfterBattle(Instance->CachedBattleMemory.LastStoredQueue);
			bVictory = true;
			
			SetBattlePhase(EBattlePhase::Finished);
		}
		
		return;
	}


	if (!CurrentPlayer->IsAlive()) 
	{
		bFightStarted = false;
		bIsPlayerTurn = false;
		bIsAiTurn = false;

		DebugHelper::AddMessageToLog("[Turn System]: Game Over!");
		
		TryGetCurrentPlayer()->GetBattleHUD()->DisplayGameOverVisualizer();
		SetBattlePhase(EBattlePhase::Finished);
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
	CurrentPlayer->GetInGameMenu()->SetDisabled(false);
	bRequestFight = false;
	bInit = false;
	bVictory = false;
	Turn.Queue.Empty();
	Instance->CachedBattleMemory.Clear();
	BattleTurnCounter = 0;
	
	Instance->GetCurrentPlayer()->GetStatusTracker()->Reset();
	Instance->GetCurrentPlayer()->GetBattleHUD()->RetrieveNotUsedBullets();
	
	const FRuntimeStats& LiveResults = CurrentPlayer->GetRuntimeStats();
	
	Instance->GetRuntimeStats().CurrentHealth = LiveResults.CurrentHealth;
	Instance->GetRuntimeStats().Experience = LiveResults.Experience;
	Instance->GetPlayerStats()->RuntimeStats = Instance->GetRuntimeStats();
}

void UTurnBasedSystem::Reload()
{
	RequestFight(false);
	bInit = false;
	bVictory = false;
	bIsPlayerTurn = false;
	bIsAiTurn = false;
	Instance->GetCurrentPlayer()->GetStatusTracker()->Reset();
	Instance->GetRuntimeStats().ApModifier = 1;
	CurrentPlayer->GetRuntimeStats().Reset(Instance->GetPlayerStats()->MaxHealth);
	Instance->GetRuntimeStats().CurrentHealth = CurrentPlayer->GetRuntimeStats().CurrentHealth;
	Instance->GetCurrentPlayer()->GetBattleHUD()->SetBulletSetupFinished(false);
	Turn.CurrentTurn = 0;
	BattleTurnCounter = 0;
}

EBattlePhase UTurnBasedSystem::GetBattlePhase() const
{
	return BattlePhase;
}

void UTurnBasedSystem::SetBattlePhase(const EBattlePhase& Phase)
{
	BattlePhase = Phase;
}

void UTurnBasedSystem::PrePrepareToBattle()
{
	if (BattlePhase != EBattlePhase::Preparation)
	{
		DebugHelper::LogMessage(10, FColor::Red, "Can't pre-prepare the fight");
		return;
	}

	SetBattlePhase(EBattlePhase::Running);
	bRequestFight = true;
}

TArray<AICC_Actor*> UTurnBasedSystem::GetCopyQueue() const
{
	return CopyQueue;
}
