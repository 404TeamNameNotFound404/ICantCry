// Fill out your copyright notice in the Description page of Project Settings.


#include "EJealousy.h"


// Sets default values
AEJealousy::AEJealousy()
{
	bIsEJealousy = true;
	Type = EMobType::MobJealousy;
}

// Called when the game starts or when spawned
void AEJealousy::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEJealousy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



