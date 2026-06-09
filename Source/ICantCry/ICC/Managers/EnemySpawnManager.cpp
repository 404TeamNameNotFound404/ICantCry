// Fill out your copyright notice in the Description page of Project Settings.
#include "EnemySpawnManager.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Algo/RandomShuffle.h"
#include "ICantCry/ICC/Actors/AI/ICC_AIController.h"
#include "EngineUtils.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


// Sets default values
AEnemySpawnManager::AEnemySpawnManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}


// Called when the game starts or when spawned
void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AESpawner> It(GetWorld()); It; ++It)
	{
		SpawnPoints.Add(*It);
	}
}

void AEnemySpawnManager::SpawnRandomEnemy()
{
	if (EnemyList.Num() == 0 || SpawnPoints.Num() == 0)
	{
		DebugHelper::LogError("Enemy List is 0 or Spawn points are 0");
		return;
	}

	Spawn();
}

void AEnemySpawnManager::RespawnEnemy(TSubclassOf<AMob> Class, const FVector& Location, const FRotator& Rotation)
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AMob* Emotion = GetWorld()->SpawnActor<AMob>(
		Class,
		Location,
		Rotation,
		SpawnParams);

	if (!Emotion->GetController())
	{
		FActorSpawnParameters AISP;
		AISP.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AICC_AIController* Controller = GetWorld()->SpawnActor<AICC_AIController>(
			Emotion->AIControllerClass, Emotion->GetActorLocation(), Emotion->GetActorRotation(), AISP);

		Controller->Possess(Emotion);
	}

	Emotion->SetIsRespawned(true);
}

void AEnemySpawnManager::ResetBattle(AMob* Emotion)
{
	if (!Emotion)
	{
		return;
	}

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(Emotion->GetGameInstance());

	AICC_AIController* AIController = Cast<AICC_AIController>(Emotion->GetController());

	checkf(AIController, TEXT("AIController is null in ResetEnemy"));

	UCharacterMovementComponent* MoveCmp = Emotion->GetCharacterMovement();

	AIController->StopMovement();

	if (AIController->BrainComponent)
	{
		AIController->BrainComponent->StopLogic(TEXT("Battle Reset"));
	}

	if (MoveCmp)
	{
		MoveCmp->StopMovementImmediately();
		MoveCmp->DisableMovement();
	}

	Emotion->SetMinigameHasStarted(false);
	Emotion->SetIsBusy(false);
	Emotion->SetIsReadyToPlay(true);

	Emotion->SetActorHiddenInGame(false);
	Emotion->SetActorEnableCollision(true);
	Emotion->SetActorTickEnabled(true);

	Emotion->GetStats().bAlive = true;
	Emotion->GetStats().Health = Emotion->GetData()->MaxHealth;
	Emotion->GetHealthBar()->Restore(Emotion->GetData()->MaxHealth);

	Emotion->SetActorLocationAndRotation(
		Emotion->GetAIMemory().DefaultBattleLocation,
		Emotion->GetAIMemory().DefaultBattleOrientation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics
	);

	if (AIController->GetPawn() != Emotion)
	{
		AIController->Possess(Emotion);
	}

	if (MoveCmp)
	{
		MoveCmp->SetMovementMode(MOVE_Walking);
	}

	if (AIController->BrainComponent)
	{
		AIController->BrainComponent->RestartLogic();
	}

	Instance->GetCurrentPlayer()->GetStats()->CurrentHealth = Instance->GetPlayerStats()->MaxHealth;

	Instance->GetCurrentPlayer()->GetBattleHUD()->ResetHealth();
	Instance->GetCurrentPlayer()->GetBattleHUD()->ResetAp();
}

void AEnemySpawnManager::Spawn()
{
	const int32 Aleatory = RandomSpawn(GetRandomSpawnType());

	TArray<AESpawner*> ValidSpawnPoints;
	for (AESpawner* Point : SpawnPoints)
	{
		if (IsValid(Point))
		{
			ValidSpawnPoints.Add(Point);
		}
	}

	if (Aleatory > ValidSpawnPoints.Num())
	{
		DebugHelper::LogError("Not enough valid spawn points!");
		return;
	}

	Algo::RandomShuffle(ValidSpawnPoints);

	for (int32 i = 0; i < Aleatory; ++i)
	{
		AESpawner* SpawnPoint = ValidSpawnPoints[i];

		const FVector SpawnLocation = SpawnPoint->GetActorLocation();
		const FRotator SpawnRotation = SpawnPoint->GetActorRotation();

		const int32 EnemyIndex = FMath::RandRange(0, EnemyList.Num() - 1);
		TSubclassOf<AMob> SelectedEnemyClass = EnemyList[EnemyIndex];

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMob* Enemy = GetWorld()->SpawnActor<AMob>(SelectedEnemyClass, SpawnLocation, SpawnRotation, SpawnParams);
		Memory.Register(SelectedEnemyClass, SpawnLocation, SpawnRotation);
		Memory.EmotionsSpawned.Add(Enemy);

		if (Enemy)
		{
			//AAIController* AIController = Cast<AAIController>(Enemy->GetController());
			AICC_AIController* AIController = Cast<AICC_AIController>(Enemy->GetController());

			if (!AIController)
			{
				DebugHelper::LogError("Enemy has no AIController even after spawn!");
			}
			else
			{
				AIController->RunBehaviorTree(Enemy->GetBehaviorTree());
			}
		}
		else
		{
			DebugHelper::LogError("Failed to spawn enemy!");
		}
	}
}

int32 AEnemySpawnManager::RandomSpawn(const ESpawnCounter& SpawnType)
{
	switch (SpawnType)
	{
	case One:
		return 1;
	case Two:
		return 2;
	case Three:
		return 3;
	default:
		return 1;
	}
}

int32 AEnemySpawnManager::RandomSpawnPoint()
{
	return FMath::RandRange(0, SpawnPoints.Num() - 1);
}

ESpawnCounter AEnemySpawnManager::GetRandomSpawnType()
{
	int32 RandomIndex = FMath::RandRange(0, static_cast<int32>(ESpawnCounter::LAST) - 1);
	return static_cast<ESpawnCounter>(RandomIndex);
}
