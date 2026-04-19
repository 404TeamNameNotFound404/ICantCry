// Fill out your copyright notice in the Description page of Project Settings.


#include "ICCSafeZone.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


// Sets default values
AICCSafeZone::AICCSafeZone()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	
	SafeArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeArea"));
	RootComponent = SafeArea;
	SafeArea->SetCollisionProfileName(TEXT("Trigger"));
}

// Called when the game starts or when spawned
void AICCSafeZone::BeginPlay()
{
	Super::BeginPlay();
	
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	SafeArea->OnComponentBeginOverlap.AddDynamic(this, &AICCSafeZone::OnOverlapBegin);
	SafeArea->OnComponentEndOverlap.AddDynamic(this, &AICCSafeZone::OnOverlapEnd);
	
	TArray<AActor*> OverlappingActors;
	SafeArea->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Cast<AICC_Player>(Actor))
		{
			bInSafeArea = true;
			DebugHelper::LogWarning("Player detected inside Safe Zone at Start!");
			break; 
		}
	}
}

void AICCSafeZone::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	if (const AICC_Player* Player = Cast<AICC_Player>(OtherActor); !Player)
	{
		return;
	}
	
	bInSafeArea = true;
	
	Handler->SetSafeZone(bInSafeArea);
	
	DebugHelper::LogWarning("Inside Safe area");
}

void AICCSafeZone::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (const AICC_Player* Player = Cast<AICC_Player>(OtherActor); !Player)
	{
		return;
	}
	
	bInSafeArea = false;
	Handler->SetSafeZone(bInSafeArea);
	
	DebugHelper::LogWarning("Outside Safe area");
}

bool AICCSafeZone::IsPlayerInSafeZone() const
{
	return bInSafeArea;
}

