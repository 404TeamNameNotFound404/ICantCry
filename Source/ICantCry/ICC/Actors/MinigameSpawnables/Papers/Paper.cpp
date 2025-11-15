// Fill out your copyright notice in the Description page of Project Settings.


#include "Paper.h"

#include "ICantCry/ICC/Input/ICC_PlayerController.h"


// Sets default values
APaper::APaper()
{
	PrimaryActorTick.bCanEverTick = true;
	Tags.Add(FName("Challenge"));
}

// Called when the game starts or when spawned
void APaper::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
	CurrentLocation = StartLocation;
}
	
// Called every frame
void APaper::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBeingDragged)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		FVector WorldLocation, WorldDirection;

		if (PC && PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection))
		{
			FVector NewLocation = WorldLocation + WorldDirection * 500.0f; // Project forward
			NewLocation.Z = StartLocation.Z + 10.0f; // Lock Z to a fixed height
			SetActorLocation(NewLocation);
		}
	}
}

void APaper::Lift(const FVector& LiftLocation)
{
	SetActorLocation(LiftLocation);
	CurrentLocation = LiftLocation;
	bLifted = true;
	bIsSwapped = false;
}

void APaper::Release()
{
	SetActorLocation(StartLocation);
	bIsBeingDragged = false;
	bLifted = false;
	bIsSwapped = false;
}

FVector APaper::GetStartLocation() const
{
	return StartLocation;
}

FVector APaper::GetCurrentLocation() const
{
	return CurrentLocation;
}

bool APaper::GetIsLifted() const
{
	return bLifted;
}

bool APaper::GetIsSwapped() const
{
	return bIsSwapped;
}


void APaper::SetIsDragged(const bool& Value)
{
	bIsBeingDragged = Value;
}

