// Fill out your copyright notice in the Description page of Project Settings.


#include "EFear.h"


// Sets default values
AEFear::AEFear()
{
	bIsEFear = true;
	Type = EMobType::MobFear;
}

// Called when the game starts or when spawned
void AEFear::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEFear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

