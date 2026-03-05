// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_GiveRewards.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"



void UEvent_GiveRewards::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	if (!Player) return;

	UPlayerStats* Stats = Player->GetStats();
	if (!Stats) return;

	// Applichiamo l'esperienza usando la tua nuova struttura RuntimeStats
	if (ExperienceToAdd > 0.0f)
	{
		Stats->RuntimeStats.Experience += ExperienceToAdd;
	}

	// Applichiamo la cura
	if (HealthToRestore > 0.0f)
	{
		Stats->CurrentHealth = FMath::Clamp(Stats->CurrentHealth + HealthToRestore, 0.0f, Stats->MaxHealth);
	}
}
