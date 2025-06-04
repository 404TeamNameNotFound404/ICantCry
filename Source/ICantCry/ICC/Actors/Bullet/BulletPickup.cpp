// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletPickup.h"

// Sets default values
ABulletPickup::ABulletPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	USphereComponent* Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    Sphere->InitSphereRadius(50.f);
    Sphere->SetCollisionProfileName("OverlapAllDynamic");
    RootComponent = Sphere;

    Sphere->OnComponentBeginOverlap.AddDynamic(this, &ABulletPickup::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ABulletPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABulletPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABulletPickup::OnOverlapBegin(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    if (AICC_Player* Player = Cast<AICC_Player>(OtherActor))
    {
        if (UInventoryManager* InvManager = Player->GetInventoryManager())
        {
            InvManager->AddItem(EItemType::Bullet, BulletData, TArray<FEssence>(), FRecipe(), Quantity);
            Destroy();
        }
    }
}

