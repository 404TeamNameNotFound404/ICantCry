// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_CollectItem.h"
#include "ICantCry/ICC/Mechanics/Pickups/BasePickup.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UEvent_CollectItem::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	// Il 'Context' in questo caso è l'oggetto che ha l'InteractionComponent
	ABasePickup* PickupItem = Cast<ABasePickup>(Context);

	if (Player && PickupItem)
	{
		// Chiamiamo la funzione Collect della classe BasePickup
		PickupItem->Collect(Player);
	}
}