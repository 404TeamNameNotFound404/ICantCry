// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_DropItem.h"

void UEvent_DropItem::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	AActor* NPC = Cast<AActor>(Context);
	if (!NPC || !ItemClass) return;

	// Calcoliamo una posizione davanti all'NPC
	FVector SpawnLoc = NPC->GetActorLocation() + (NPC->GetActorForwardVector() * 100.0f);
	
	// Spawniamo l'oggetto nel mondo
	NPC->GetWorld()->SpawnActor<AActor>(ItemClass, SpawnLoc, FRotator::ZeroRotator);
}