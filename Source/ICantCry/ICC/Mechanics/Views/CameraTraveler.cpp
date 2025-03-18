// Fill out your copyright notice in the Description page of Project Settings.
#include "CameraTraveler.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
ACameraTraveler::ACameraTraveler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxComponents = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponents->OnComponentBeginOverlap.AddDynamic(this, &ACameraTraveler::OnOverlapBegin);
	BoxComponents->OnComponentEndOverlap.AddDynamic(this, &ACameraTraveler::OnOverlapEnd);
}

// Called when the game starts or when spawned
void ACameraTraveler::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACameraTraveler::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									  const FHitResult& SweepResult)
{
	checkf(Player, TEXT("Player not defined in CameraTraveler constructor, abort"));
	
	if (!Player || OtherActor != Player || bPlayerOverlapped)
	{
		return;
	}

	// Mark that the player has overlapped
	bPlayerOverlapped = true;
	
	if (Player->GetWorldCamera() && Player)
	{
		Player->GetWorldCamera()->MoveToNextWaypoint();
		DebugHelper::LogSuccess("Move to waypoint");
	}

	if (bIsOneShot)
	{
		BoxComponents->Deactivate();
	}
	
}


void ACameraTraveler::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!Player || OtherActor != Player)
	{
		return;
	}

	bPlayerOverlapped = false;
	
}

void ACameraTraveler::SetIsOneShot(const bool& Value)
{
	bIsOneShot = Value;
}



