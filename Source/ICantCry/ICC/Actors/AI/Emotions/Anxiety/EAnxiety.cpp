// Fill out your copyright notice in the Description page of Project Settings.
#include "EAnxiety.h"


// Sets default values
AEAnxiety::AEAnxiety()
{
	bIsEAnxiety = true;
	Type = EMobType::MobAnxiety;
}

// Called when the game starts or when spawned
void AEAnxiety::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEAnxiety::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


