// Fill out your copyright notice in the Description page of Project Settings.


#include "RecipePickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../Source/ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"

FRecipe ARecipePickup::CurrentRecipe;

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
    
    const FName TargetName = "AngerDv";
    if (const FRecipe* TargetRecipe = Recipes->FindRow<FRecipe>(TargetName, TEXT("Looking for")); 
        TargetRecipe)
    {
        Self = *TargetRecipe;
        Self.RequiredBlueprintType = RecipeType;
        DebugHelper::LogSuccess("Recipe " + Self.GetName(Self.RequiredBlueprintType) + " found");
    }
}


void ARecipePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                   bool bFromSweep, const FHitResult& SweepResult)
{

    AICC_Player* Player = Cast<AICC_Player>(OtherActor);
    if (Player)
    {
        FInventory Inventory = Player->GetPlayerInventory();
        Player->SetPlayerInventory(Inventory);
        
        // if (!Inventory.OwnedBlueprints.Contains(RecipeType))
        // {
        //     DebugHelper::LogError("");
        //     Inventory.OwnedBlueprints.Add(RecipeType);
        //     Player->SetPlayerInventory(Inventory);
        // }

        Player->SetIsPickedUp(true);

        OnPickedUp(Player);
        Destroy();
    }
}


void ARecipePickup::OnPickedUp(AActor* OtherActor)
{
    DebugHelper::LogError("OnPickedUp");
    
    if (AICC_Player* Player = Cast<AICC_Player>(OtherActor))
    {
        // if (UInventoryManager* InvMgr = Player->GetInventoryManager())
        // {
        //     InvMgr->AddRecipe(Self);
        //     CurrentRecipe = Self;
        // }

        Player->GetInventoryManager()->RecipeUnlocked.Broadcast(Self.RequiredBlueprintType);
        
        Player->SetIsPickedUp(false);
    }
}



// Called every frame
void ARecipePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

