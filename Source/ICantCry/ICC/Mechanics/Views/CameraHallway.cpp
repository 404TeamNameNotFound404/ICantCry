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
	
	Rooms = {"LivingRoom", "Hallway", "MaxRoom", "Bedroom", "Office"};
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
		RenderSeen(RoomTag);
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
	if (!Player || !Player->GetWorldCamera()) return;
	
	Counter = Player->GetWorldCameraCounter();
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	
	if (Counter == 0)
	{
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraFixedWaypoint);
        
		if (bEnableWorldCamera)
		{
			Player->GetWorldCamera()->SetbDefaultCamera(true);
			Controller->SetViewTargetWithBlend(Player->GetWorldCamera(), 0.5f);
		}
		else
		{
			Controller->SetViewTargetWithBlend(Player, 0.0f);
		}

		if (bPlayerMustTeleport && InEntry.IsValid())
		{
			Player->SetActorLocationAndRotation(InEntry->GetActorLocation(), InEntry->GetActorRotation());
		}
		
		Player->SetWorldCameraCounter(1);
	}
	
	else if (Counter == 1)
	{
		Player->GetWorldCamera()->SnapToFixedWaypoint(CameraBackWaypoint);

		if (bEnableWorldCamera)
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
		RenderSeen(RoomTag);
	}
}

void ACameraHallway::ToggleRoom(const FName& Room, const bool& bRenderRoom)
{
	TArray<AActor*> SpawnedRooms;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Room,  SpawnedRooms);
	
	for (AActor* R : SpawnedRooms)
	{
		if (!R)
		{
			DebugHelper::LogError("Room is invalid");
			continue;
		}
		
		R->SetActorHiddenInGame(!bRenderRoom);
		R->SetActorEnableCollision(bRenderRoom);
		R->SetActorTickEnabled(bRenderRoom); // I dont think we'll need this but Im gonna keep it for now
		
		DebugHelper::LogMessage(6, FColor::Cyan, FString(RoomTag.ToString() + FString(bRenderRoom ? "Hidden" : "Shown")));
	}
}

void ACameraHallway::RenderSeen(const FName& TargetToHide)
{
	for (FName& Room : Rooms)
	{
		if (Room == TargetToHide)
		{
			ToggleRoom(Room, true);
		}
		else
		{
			ToggleRoom(Room, false);
		}
	}
}


// Called every frame
void ACameraHallway::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

