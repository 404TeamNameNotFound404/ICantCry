// Fill out your copyright notice in the Description page of Project Settings.
#include "ICCRoomHandler.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"

// Sets default values
AICCRoomHandler::AICCRoomHandler()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

// Called when the game starts or when spawned
void AICCRoomHandler::BeginPlay()
{
	Super::BeginPlay();
	
	HideAll();
	
	InitEvent();
}

void AICCRoomHandler::SetRoomVisible(AActor* Room, const bool& bVisible)
{
	if (!Room) return;
	
	Room->SetActorHiddenInGame(!bVisible);
	Room->SetActorEnableCollision(bVisible);
	Room->SetActorTickEnabled(bVisible);
	
	TArray<AActor*> AttachedChildren;
	Room->GetAttachedActors(AttachedChildren);
	
	const FString DisplayedRoom = "Room " + Room->GetActorLabel() + (bVisible ? " Hidden" : " Showed");
	
	for (AActor* Child : AttachedChildren)
	{
		if (!Child) continue;
		Child->SetActorHiddenInGame(!bVisible);
		Child->SetActorEnableCollision(bVisible);
		Child->SetActorTickEnabled(bVisible);
	}
	
	DebugHelper::LogMessage(6, FColor::White, DisplayedRoom + "  by the people");
}

TArray<TObjectPtr<AActor>> AICCRoomHandler::GetRooms()
{
	return Rooms;
}

void AICCRoomHandler::InitEvent()
{
	if (!OnRoomChanged.IsBoundToObject(this))
	{
		OnRoomChanged.AddUObject(this, &AICCRoomHandler::HideOthers);
	}
}

void AICCRoomHandler::HideOthers(AActor* Other)
{
	for (AActor* Room : Rooms)
	{
		if (Room == Other)
		{
			SetRoomVisible(Room, true);
		}
		else
		{
			SetRoomVisible(Room, false);
		}
	}
}

bool AICCRoomHandler::HideAll()
{
	if (Rooms.IsEmpty()) return false;
	
	for (AActor* Room: Rooms)
	{
		if (!Room) continue;
		if (Room->ActorHasTag("Hallway")) continue;
		
		Room->SetActorHiddenInGame(true);
		Room->SetActorEnableCollision(false);
		Room->SetActorTickEnabled(false);
		
		TArray<AActor*> AttachedChildren;
		Room->GetAttachedActors(AttachedChildren);
		
		for (AActor* Child : AttachedChildren)
		{
			if (!Child) continue;
			
			Child->SetActorHiddenInGame(true);
			Child->SetActorEnableCollision(false);
			Child->SetActorTickEnabled(false);
		}
	}
	
	return true;
}


