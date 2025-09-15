// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTeleportObj.h"


// Sets default values
APlayerTeleportObj::APlayerTeleportObj()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void APlayerTeleportObj::BeginPlay()
{
	Super::BeginPlay();
	
}


