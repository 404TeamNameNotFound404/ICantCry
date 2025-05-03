// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusTracker.h"


// Sets default values for this component's properties
UStatusTracker::UStatusTracker()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	bIsOwnerAfflicted = false;
}


// Called when the game starts
void UStatusTracker::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UStatusTracker::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UStatusTracker::IsAfflicted()
{
	return false;
}

void UStatusTracker::SetStatusApplied(const bool& Applied)
{
	bIsOwnerAfflicted = Applied;
}

