// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_GiveRewards.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"



void UEvent_GiveRewards::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
	if (!Player) return;
	auto Stats = Player->GetStats();
	if (!Stats) return;

    // upd experience in runtimestats container
	if (ExperienceToAdd > 0.0f)
	{
		Stats->RuntimeStats.Experience += ExperienceToAdd;
	}

    // apply heal and clamp to max capacity
	if (HealthToRestore > 0.0f)
	{
		Stats->CurrentHealth = FMath::Clamp(Stats->CurrentHealth + HealthToRestore, 0.0f, Stats->MaxHealth);
	}
}
