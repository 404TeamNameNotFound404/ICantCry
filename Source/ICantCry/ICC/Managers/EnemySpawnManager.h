// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Math/UnrealMathUtility.h"
#include "../Mechanics/Core/Data/EnemyDatas.h"
#include "../Actors/AI/Mob.h"
#include "EnemySpawnManager.generated.h"

UCLASS()
class ICANTCRY_API AEnemySpawnManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawnManager();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnRandomEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<AMob>> EnemyList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> SpawnPoints;

	UPROPERTY()
	int32 CounterEnemyIndex = 1;

	void SpawnOneEnemy(TSubclassOf<AMob> Mob, FVector Position,  FRotator Rotation);
	void SpawnTwoEnemy(TSubclassOf<AMob> Mob, FVector Position,  FVector Position2, FRotator Rotation, FRotator Rotation2);
	void SpawnThreeEnemy(TSubclassOf<AMob> Mob, FVector Position,  FVector Position2, FVector Position3, FRotator Rotation, FRotator Rotation2, FRotator Rotation3 );



};
