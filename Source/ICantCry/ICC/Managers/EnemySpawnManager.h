// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "../Actors/AI/Mob.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/Core/ESpawner.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleMemory/FBattleMemory.h"
#include "EnemySpawnManager.generated.h"

UENUM()
enum ESpawnCounter
{
	One,
	Two,
	Three,
	LAST  UMETA(Hidden)
};

UCLASS()
class ICANTCRY_API AEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnManager();
	
	void SpawnRandomEnemy();

	void RespawnEnemy(TSubclassOf<AMob> Class, const FVector& Location, const FRotator& Rotation);

	void ResetBattle(AMob* Emotion);

	FBattleMemory& GetMemory() { return Memory; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AMob>> EnemyList;

	UPROPERTY()
	TArray<AESpawner*> SpawnPoints;

	UPROPERTY()
	int32 CounterEnemyIndex = 1;

	void Spawn();

	int32 RandomSpawn(const ESpawnCounter& SpawnType);
	int32 RandomSpawnPoint();
	
	ESpawnCounter GetRandomSpawnType();

	UPROPERTY()
	FBattleMemory Memory;
};
