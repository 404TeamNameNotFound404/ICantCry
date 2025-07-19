// Fill out your copyright notice in the Description page of Project Settings.


#include "EDisgust.h"


// Sets default values
AEDisgust::AEDisgust()
{
	bIsEDisgust = true;
	Type = EMobType::MobDisgust;
}

// Called when the game starts or when spawned
void AEDisgust::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEDisgust::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

