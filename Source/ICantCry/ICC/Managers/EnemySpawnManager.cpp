// Fill out your copyright notice in the Description page of Project Settings.
#include "EnemySpawnManager.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "Algo/RandomShuffle.h"
#include "EngineUtils.h"


// Sets default values
AEnemySpawnManager::AEnemySpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled= false;

}


// Called when the game starts or when spawned
void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AESpawner> It(GetWorld()); It; ++It)
	{
		SpawnPoints.Add(*It);
		DebugHelper::LogSuccess( "Added " + It->GetActorLabel());
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

void AEnemySpawnManager::Spawn()
{
	const int32 Aleatory = RandomSpawn(GetRandomSpawnType());

	DebugHelper::LogMessage(5, FColor::Cyan, "Will spawn " + FString::FromInt(Aleatory) + " enemies");

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
	DebugHelper::LogError("Shuffled valid spawn points count: " + FString::FromInt(ValidSpawnPoints.Num()));

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

		if (Enemy)
		{
			DebugHelper::LogWarning("Now spawning " + Enemy->GetData()->EnemyName.ToString());

			AAIController* AIController = Cast<AAIController>(Enemy->GetController());

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
