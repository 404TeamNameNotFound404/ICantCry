// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_CollectItem.h"
#include "ICantCry/ICC/Mechanics/Pickups/BasePickup.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UEvent_CollectItem::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	// cast context to basepickup to trigger inventory/quest logic
	if (auto PickupItem = Cast<ABasePickup>(Context))
	{
		PickupItem->Collect(Player);
	}
}