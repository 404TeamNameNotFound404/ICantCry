// Fill out your copyright notice in the Description page of Project Settings.


#include "EncounterHandler.h"


// Sets default values
AEncounterHandler::AEncounterHandler() : MultiplierLocation(EPlayerLocation::School)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AEncounterHandler::SetSafeZone(const bool& bInSafeZone)
{
	this->bIsSafeZone = bInSafeZone;
}

// Called when the game starts or when spawned
void AEncounterHandler::BeginPlay()
{
	Super::BeginPlay();

	EncounterManager = NewObject<UEncounterManager>();
	EncounterManager->Initialize(GetWorld());
	EncounterManager->SetPlayerLocationMultiplier(MultiplierLocation);
	bIsSafeZone = false;
}

void AEncounterHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	EncounterManager->SetPaused(bIsSafeZone);
}

