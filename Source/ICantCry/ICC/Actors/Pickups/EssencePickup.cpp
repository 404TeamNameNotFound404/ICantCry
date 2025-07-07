// Fill out your copyright notice in the Description page of Project Settings.


#include "EssencePickup.h"

#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

// Sets default values
AEssencePickup::AEssencePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

    USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Sphere->InitSphereRadius(50.f);
    Sphere->SetCollisionProfileName("OverlapAllDynamic");
    RootComponent = Sphere;

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &AEssencePickup::OnOverlapBegin);

}

// Called when the game starts or when spawned
void AEssencePickup::BeginPlay()
{
	Super::BeginPlay();
	
    Self.EssenceType = EssenceType;
	Self.Quantity = Quantity;
}

void AEssencePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
	AICC_Player* Player = Cast<AICC_Player>(Controller->GetPawn());

	Player->GetInventoryManager()->AddEssence2(Self);
	Destroy();
}


