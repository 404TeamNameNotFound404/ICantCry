// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_Actor.h"


// Sets default values
AICC_Actor::AICC_Actor()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	StatusTracker = CreateDefaultSubobject<UStatusTracker>("StatusTracker");
}

UStatusTracker* AICC_Actor::GetStatusTracker() const
{
	return StatusTracker;
}


