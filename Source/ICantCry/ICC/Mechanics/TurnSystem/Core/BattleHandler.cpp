// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleHandler.h"


// Sets default values
ABattleHandler::ABattleHandler(): TurnBasedSystem(nullptr)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	TurnBasedSystem = NewObject<UTurnBasedSystem>();
}

// Called when the game starts or when spawned
void ABattleHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABattleHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

