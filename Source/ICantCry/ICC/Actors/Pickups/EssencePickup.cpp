// Fill out your copyright notice in the Description page of Project Settings.


#include "EssencePickup.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
AEssencePickup::AEssencePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	
}

void AEssencePickup::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (AICC_Player* Player = Cast<AICC_Player>(OtherActor))
    {
        if (UInventoryManager* Manager = Player->GetInventoryManager())
        {
            check(Manager)
            DebugHelper::LogWarning("Pickup: InventoryManager trovato!");
            Manager->AddEssence(EssenceType, Quantity);
            //Destroy();
        }
        else
        {
            DebugHelper::LogWarning("Pickup: InventoryManager é null!");
        }
    }
}

// Called every frame
void AEssencePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

