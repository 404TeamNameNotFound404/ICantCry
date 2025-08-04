// Fill out your copyright notice in the Description page of Project Settings.
#include "EAnger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


// Sets default values
AEAnger::AEAnger()
{
	bIsHealer = false;
	bEAnger = true;
	Type = EMobType::MobAnger;
}

// Called when the game starts or when spawned
void AEAnger::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEAnger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

