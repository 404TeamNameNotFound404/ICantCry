// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
ARecipePickup::ARecipePickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
    RootComponent = Collision;
    Collision->SetSphereRadius(50.f);
    Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
    Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    Mesh->SetupAttachment(RootComponent);

    Collision->OnComponentBeginOverlap.AddDynamic(this, &ARecipePickup::OnOverlapBegin);

}

// Called when the game starts or when spawned
void ARecipePickup::BeginPlay()
{
	Super::BeginPlay();
	
}


void ARecipePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{

    AICC_Player* Player = Cast<AICC_Player>(OtherActor);
    if (Player)
    {
        FInventory Inventory = Player->GetPlayerInventory(); 
        if (!Inventory.OwnedBlueprints.Contains(RecipeType))
        {
            Inventory.OwnedBlueprints.Add(RecipeType);
            Player->SetPlayerInventory(Inventory);
        }

        
        Destroy();
    }
}


void ARecipePickup::OnPickedUp(AActor* OtherActor)
{
    if (AICC_Player* Player = Cast<AICC_Player>(OtherActor))
    {
        if (UInventoryManager* InvMgr = Player->GetInventoryManager())
        {
            InvMgr->AddRecipe(RecipeType, Quantity);
            Destroy();
        }
    }
}


// Called every frame
void ARecipePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

