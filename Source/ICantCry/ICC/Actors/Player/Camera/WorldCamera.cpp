// Fill out your copyright notice in the Description page of Project Settings.
#include "WorldCamera.h"
#include "EngineUtils.h"
#include "MaterialHLSLTree.h"
#include "Camera/CameraComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
AWorldCamera::AWorldCamera()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("WorldCamera"));
	bDefaultCamera = true;
	WaypointIndex = 0;
	CurrentWaypoint = nullptr;
	PreviousWaypoint = nullptr;
}

// Called when the game starts or when spawned
void AWorldCamera::BeginPlay()
{
	Super::BeginPlay();

	if (!bDefaultCamera)
	{
		return;
	}

	for (TActorIterator<ACameraWaypoint> It(GetWorld()); It; ++It)
	{
		Waypoints.Add(*It);
		DebugHelper::LogMessage(3, FColor::White, "Added: " + It->GetName());
	}
	if (Waypoints.IsEmpty()) return;
	
	SetActorLocation(Waypoints[WaypointIndex]->GetActorLocation());
	SetActorRotation(Waypoints[WaypointIndex]->GetActorRotation());
	CurrentWaypoint = Waypoints[WaypointIndex];
	
	if (bDefaultCamera)
	{
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		checkf(PlayerController, TEXT("PlayerController is NULL"));
		PlayerController->SetViewTargetWithBlend(this, 0.0f);
	}
}


bool AWorldCamera::IsDefaultCamera()
{
	return bDefaultCamera;
}

void AWorldCamera::SetbDefaultCamera(const bool& bCondition)
{
	bDefaultCamera = bCondition;
}

int AWorldCamera::GetWaypointIndex() const
{
	return WaypointIndex;
}

void AWorldCamera::SetWaypointIndex(int index)
{
	WaypointIndex = index;
}

AActor* AWorldCamera::FindClosestWaypoint()
{
	if (Waypoints.Num() == 0)
	{
		DebugHelper::LogError("Waypoints array is empty");
		return nullptr;
	}

	AActor* ClosestWaypoint = nullptr;
	float MinDistance = FLT_MAX;

	const FVector PlayerLocation = Player->GetActorLocation();
	
	if (PreviousWaypoint && FVector::Dist(PlayerLocation, PreviousWaypoint->GetActorLocation()) < 400.0f) // 400 for testing purpose, may vary
	{
		CurrentWaypoint = PreviousWaypoint;
		PreviousWaypoint = nullptr;
		return CurrentWaypoint;
	}
	
	for (AActor* Waypoint : Waypoints)
	{
		if (!Waypoint || Waypoint == CurrentWaypoint) continue;

		const float Distance = FVector::Dist(PlayerLocation, Waypoint->GetActorLocation());

		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			ClosestWaypoint = Waypoint;
		}
	}
	
	PreviousWaypoint = CurrentWaypoint;
	CurrentWaypoint = ClosestWaypoint;

	return ClosestWaypoint;
}

// Called every frame
void AWorldCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bLerpStarted && CurrentWaypoint)
	{
		const FVector CurrentLocation = GetActorLocation();
		const FVector TargetLocation = CurrentWaypoint->GetActorLocation();
		
		const FVector InterpLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 6.0f);
		SetActorLocation(InterpLocation);
		SetActorRotation(CurrentWaypoint->GetActorRotation());
		
		if (FVector::Dist(InterpLocation, TargetLocation) < 1.0f)
		{
			bLerpStarted = false;
		}
	}
}

void AWorldCamera::MoveToNextWaypoint()
{
	if (bEnableSmoothMovement)
	{
		if (AActor* ClosestWaypoint = FindClosestWaypoint())
		{
			CurrentWaypoint = ClosestWaypoint;
			bLerpStarted = true;
		}
	}
	else
	{
		if (AActor* ClosestWaypoint = FindClosestWaypoint())
		{
			CurrentWaypoint = ClosestWaypoint;
			SetActorLocation(CurrentWaypoint->GetActorLocation());
			SetActorRotation(CurrentWaypoint->GetActorRotation());
		}
	}
}


