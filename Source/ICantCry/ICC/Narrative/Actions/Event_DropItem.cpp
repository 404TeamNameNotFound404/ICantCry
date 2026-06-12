// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_DropItem.h"

void UEvent_DropItem::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	AActor* NPC = Cast<AActor>(Context);
	if (!NPC || !ItemClass) return;

	// offset spawn loc by 100 units fwd to avoid collision with npc
	FVector SpawnLoc = NPC->GetActorLocation() + (NPC->GetActorForwardVector() * 100.0f);
	
	// Spawn the object into the world
	NPC->GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLoc, FRotator::ZeroRotator);
}