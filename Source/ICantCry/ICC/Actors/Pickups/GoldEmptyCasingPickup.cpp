// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldEmptyCasingPickup.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


// Sets default values
AGoldEmptyCasingPickup::AGoldEmptyCasingPickup()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AGoldEmptyCasingPickup::OnOverlapBegin);
}

FGoldEmptyCasing& AGoldEmptyCasingPickup::GetSelf() 
{
	return Self;
}

// Called when the game starts or when spawned
void AGoldEmptyCasingPickup::BeginPlay()
{
	Super::BeginPlay();

	QuantityCounter = Self.GetQuantity();
}


void AGoldEmptyCasingPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OnPickUp();
}

void AGoldEmptyCasingPickup::OnPickUp()
{

	UICantCryGameInstance* GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());

	const FString NameKey = Self.GetName();

	if (GameInstance->GetInventory().CasingsStored.Contains(NameKey))
	{
		DebugHelper::LogWarning("Casings already registered, updating just its quantity");

		int32 ExistingQuantity = GameInstance->GetInventory().CasingsStored[NameKey].GetQuantity();
		GameInstance->GetInventory().CasingsStored[NameKey].SetQuantity(ExistingQuantity + 1);

		DebugHelper::LogWarning("New Quantity: " + FString::FromInt(GameInstance->GetInventory().CasingsStored[NameKey].GetQuantity()) + " gold");
	}
	else
	{
		Self.SetQuantity(1);
		GameInstance->GetInventory().CasingsStored.Add(NameKey, Self);

		DebugHelper::LogWarning("Casings added with initial quantity 1");
	}
	
	GameInstance->GetInventory().GoldCasings.Add(Self);
	GameInstance->GetInventory().CasingsInInventory.Add(CasingType, Self);

	Destroy();
}


