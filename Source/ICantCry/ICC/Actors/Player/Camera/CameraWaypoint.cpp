// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraWaypoint.h"


// Sets default values
ACameraWaypoint::ACameraWaypoint()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void ACameraWaypoint::BeginPlay()
{
	Super::BeginPlay();
	
}


