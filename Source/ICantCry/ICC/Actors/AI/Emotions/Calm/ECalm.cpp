// Fill out your copyright notice in the Description page of Project Settings.
#include "ECalm.h"

// Sets default values
AECalm::AECalm()
{
	bIsECalm = true;
	Type = EMobType::MobCalm;
}

// Called when the game starts or when spawned
void AECalm::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AECalm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


