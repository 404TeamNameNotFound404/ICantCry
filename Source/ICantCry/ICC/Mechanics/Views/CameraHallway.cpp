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
	if (!OtherActor)
		return;

	if (!Player)
	{
		Player = Cast<UICantCryGameInstance>(GetGameInstance())->GetCurrentPlayer();
	}

	if (OtherActor != Player)return;
	if (bPlayerOverlapped || bTransitioning) return;
	
	bPlayerOverlapped = true;
	
	if (bSnap)
	{
		bTransitioning = true;
		Snap();
		ToggleRoom(RoomId);

		GetWorldTimerManager().SetTimer(
			TransitionTimerHandle,
			[this]()
			{
				bTransitioning = false;
			},
			0.18f,
			false
		);
	}
	else
	{
		DebugHelper::LogSuccess("Player overlapped");
		
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
			Cast<UICantCryGameInstance>(GetGameInstance())->GetPlayerRuntimeData().LastWorldCameraPosition = CameraFixedWaypoint->GetActorLocation();
			Cast<UICantCryGameInstance>(GetGameInstance())->GetPlayerRuntimeData().LastWorldCameraRotation = CameraFixedWaypoint->GetActorRotation();
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
	if (!Player || !Player->GetWorldCamera()) return;
	
	if (bRememberCounter)
	{
		Counter = CachedCounter;
	}
	else
	{
		Counter = Player->GetWorldCameraCounter();
	}
	
	// Counter = Player->GetWorldCameraCounter();
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	if (Counter == 0)
	{
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraFixedWaypoint);
        
		if (bEnableWorldCamera)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.5f);
			Cast<UICantCryGameInstance>(GetGameInstance())->GetPlayerRuntimeData().LastWorldCameraPosition = CameraFixedWaypoint->GetActorLocation();
			Cast<UICantCryGameInstance>(GetGameInstance())->GetPlayerRuntimeData().LastWorldCameraRotation = CameraFixedWaypoint->GetActorRotation();
		}
		else
		{
			Controller->SetViewTargetWithBlend(Player, 0.0f);
		}

		if (bPlayerMustTeleport && InEntry.IsValid())
		{
			const FVector TargetLocation = InEntry->GetActorLocation();
			const FRotator TargetRotation = InEntry->GetActorRotation();

			Player->SetActorLocationAndRotation(
				TargetLocation,
				TargetRotation,
				false,
				nullptr,
				ETeleportType::TeleportPhysics
			);
			
			//Player->SetActorLocationAndRotation(InEntry->GetActorLocation(), InEntry->GetActorRotation());
		}
		
		Player->SetWorldCameraCounter(1);
		
		if (bRememberCounter)
		{
			CachedCounter = 1;
		}
	
	}
	
	else if (Counter == 1)
	{
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraBackWaypoint);

		if (bEnableWorldCameraOnExit)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), CameraBlendSpeed);
		}
		else
		{
			Player->GetWorldCamera()->SetbDefaultCamera(false);
			Controller->SetViewTargetWithBlend(Player, 0.0f);
		}

		if (bPlayerMustTeleport && OutEntry.IsValid())
		{
			Player->SetActorLocationAndRotation(OutEntry->GetActorLocation(), OutEntry->GetActorRotation());
		}
		
		Player->SetWorldCameraCounter(0);
		
		
		if (bRememberCounter)
		{
			CachedCounter = 0;
		}
	}
}

void ACameraHallway::ToggleRoom(const FName& RoomTag)
{
	if (!RoomHandler) return;

	AActor* TargetRoom = nullptr;

	for (AActor* Room : RoomHandler->GetRooms())
	{
		if (!Room) continue;

		if (Room->ActorHasTag(RoomTag))
		{
			TargetRoom = Room;
			break;
		}
	}

	if (TargetRoom)
	{
		RoomHandler->OnRoomChanged.Broadcast(TargetRoom);
	}
}

// Called every frame
void ACameraHallway::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

