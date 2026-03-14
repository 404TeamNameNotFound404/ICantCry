// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayEvent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


// Lasciamo l'implementazione base vuota. 
// Essendo un BlueprintNativeEvent, questa funzione verrà sovrascritta nei Blueprint 
// o nelle classi C++ figlie come Event_GiveRewards.
void UGameplayEvent::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	// Logica base opzionale (vuota)
}

