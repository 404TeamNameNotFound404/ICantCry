// Fill out your copyright notice in the Description page of Project Settings.
#include "CameraHallway.h"
#include "EngineUtils.h"
#include "LevelSequencePlayer.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
ACameraHallway::ACameraHallway()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACameraHallway::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACameraHallway::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACameraHallway::BeginPlay()
{
	Super::BeginPlay();

	checkf(CameraFixedWaypoint, TEXT("FixedWaypoint has not been assigned in editor!"))

	for (TActorIterator<ACameraTraveler> It(GetWorld()); It; ++It)
	{
		Travelers.Add(*It);
	}
}

void ACameraHallway::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	checkf(Player, TEXT("Player on CameraHallway.cpp is not initialized"));
	if (!Player || OtherActor != Player || bPlayerOverlapped)
	{
		DebugHelper::LogWarning("Only player can overlap with camera or overlap already handled");
		return;
	}
	
	bPlayerOverlapped = true;
	
	if (bSnap)
	{
		Snap();
	}
	else
	{
		DebugHelper::LogSuccess("Player overlapped");

		// Mark that the player has overlapped
		//bPlayerOverlapped = true;

		Counter = Player->GetWorldCameraCounter();
	
		if (Player->GetWorldCamera() && Player && Counter == 0)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(false);
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			checkf(PlayerController, TEXT("PlayerController is NULL"));
			PlayerController->SetViewTargetWithBlend(Player, 0.0f);
			DebugHelper::LogSuccess("Set Target to Player Camera");
			Counter++;
			Player->SetWorldCameraCounter(Counter);
		}
	
		else if (Player->GetWorldCamera() && Player  && Counter == 1)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			checkf(PlayerController, TEXT("PlayerController is NULL"));
			PlayerController->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.0f);
			Counter = 0;
			Player->SetWorldCameraCounter(Counter);
			Player->GetWorldCamera()->SnapToFixedWaypoint(CameraFixedWaypoint);
		}
	}
}

void ACameraHallway::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Player || OtherActor != Player)
	{
		DebugHelper::LogWarning("Only player can end overlap with camera");
		return;
	}
	
	bPlayerOverlapped = false;
}

void ACameraHallway::Snap()
{
	Counter = Player->GetWorldCameraCounter();

	if (Player->GetWorldCamera() && Player && Counter == 0)
	{
		DebugHelper::LogSuccess("Snap To fixed");
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraFixedWaypoint);
		AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
		Player->GetWorldCamera()->SetbDefaultCamera(true);
		Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.0f);
		Counter++;
		Player->SetWorldCameraCounter(Counter);

		if (bEnableWorldCamera)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.5f);
		}
		else
		{
			Player->GetWorldCamera()->SetbDefaultCamera(false);
			Controller->SetViewTargetWithBlend(Player, 0.0f);
			DebugHelper::LogMessage(6, FColor::Orange, "Moving");
		}

		if (bPlayerMustTeleport)
		{
			Player->SetActorLocation(InEntry.Get()->GetActorLocation());
			Player->SetActorRotation(InEntry.Get()->GetActorRotation());
		}
	}
	else if (Player->GetWorldCamera() && Player && Counter == 1)
	{
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraBackWaypoint);
		AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

		if (bEnableWorldCamera)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.5f);
		}
		else
		{
			Player->GetWorldCamera()->SetbDefaultCamera(false);
			Controller->SetViewTargetWithBlend(Player, 0.0f);
			DebugHelper::LogMessage(6, FColor::Orange, "Moving");
		}

		Counter = 0;
		Player->SetWorldCameraCounter(Counter);

		if (bPlayerMustTeleport)
		{
			Player->SetActorLocation(OutEntry.Get()->GetActorLocation());
			Player->SetActorRotation(OutEntry.Get()->GetActorRotation());
		}
	}
}


// Called every frame
void ACameraHallway::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

