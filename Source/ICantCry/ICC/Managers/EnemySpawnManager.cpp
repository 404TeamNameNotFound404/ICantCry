// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawnManager.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


// Sets default values
AEnemySpawnManager::AEnemySpawnManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled= false;

}

void AEnemySpawnManager::SpawnRandomEnemy()
{
	
	if (EnemyList.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Data Asset "));
		return;
	}

	
	if (SpawnPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spawn point"));
		return;
	}

	// RIFAI I RANDOM IN MODO TALE CHE NON SPAWNI SOLO UN ENEMY MA POSSONO SPAWNARE 1 O PIù ENEMY CONTEMPORANEAMENTE
	// RIFAI I RANDOM IN MODO TALE CHE IN BASE ALLA QUANTITA DI ENEMY SPAWNATA, DEVONO VENERIRE MESSI SU UNO O PIU SPAWN 
	// FAI TUTTO NEL METODO SpawnOneEnemy
	
	//get random enemy
	int32 EnemyIndex = FMath::RandRange(0, EnemyList.Num() - 1);
    TSubclassOf<AMob> SelectedEnemyClass = EnemyList[EnemyIndex];

	// Get a random spawn point
	int32 SpawnIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
	const FVector SpawnLocation = SpawnPoints[SpawnIndex]->GetActorLocation();
	const FRotator SpawnRotation = SpawnPoints[SpawnIndex]->GetActorRotation(); 

	// Spawn enemy 
	//AMob* SpawnedEnemy = GetWorld()->SpawnActor<AMob>(SelectedEnemyClass, SpawnLocation, SpawnRotation);

	//
	CounterEnemyIndex = FMath::RandRange(1,3);

	switch(CounterEnemyIndex)
	{
		case 1: 
			SpawnOneEnemy(SelectedEnemyClass, SpawnLocation, SpawnRotation);
			break;

		case 2:
		
	}

	
}

// IN THE IA CLASS( ENEMY CLASS) ADD THIS 
// Reference to Enemy Data Asset
// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data")
// UEnemyDatas* EnemyData;

// Get EnemyData
// UEnemyDatas* GetData() const { return EnemyData; }


// Called when the game starts or when spawned
void AEnemySpawnManager::BeginPlay()
{
	Super::BeginPlay();
	SpawnRandomEnemy();
	
}

// Called every frame
void AEnemySpawnManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AEnemySpawnManager::SpawnOneEnemy(TSubclassOf<AMob> Mob, FVector Position,  FRotator Rotation)
{
	AMob* SpawnedEnemy = GetWorld()->SpawnActor<AMob>(Mob, Position, Rotation);

			if (!SpawnedEnemy->GetData())
			{
				UE_LOG(LogTemp, Warning, TEXT(" data assets is null"));
				return;
			}


			if (SpawnedEnemy)
			{
				DebugHelper::LogWarning("Enemy Valid");

				// get enemydata
				UEnemyDatas* Data = SpawnedEnemy->GetData();
				
				if (Data)
				{
					UE_LOG(LogTemp, Log, TEXT("Spawned enemy: %s with HP: %f and AP: %f"), 
						*Data->EnemyName.ToString(), 
						Data->Health, 
						Data->AbilityPoints);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("EnemyData is missing on spawned enemy!"));

				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Spawn failed!"));
			}
}

void AEnemySpawnManager::SpawnTwoEnemy(TSubclassOf<AMob> Mob, FVector Position,  FVector Position2, FRotator Rotation, FRotator Rotation2)
{
	AMob* SpawnedEnemy = GetWorld()->SpawnActor<AMob>(Mob, Position, Rotation);
	AMob* SpawnedEnemy = GetWorld()->SpawnActor<AMob>(Mob, Position2,  Rotation2);
}

void AEnemySpawnManager::SpawnThreeEnemy(TSubclassOf<AMob> Mob, FVector Position,  FVector Position2, FVector Position3, FRotator Rotation, FRotator Rotation2, FRotator Rotation3 )
{
}
