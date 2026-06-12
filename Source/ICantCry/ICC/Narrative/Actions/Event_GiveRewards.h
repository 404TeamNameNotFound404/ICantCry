// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "Event_GiveRewards.generated.h"

/**
 * CLASS: UEvent_GiveRewards
 * DESCRIPTION: Grants specific player rewards like health or experience points.
 */
UCLASS(DisplayName = "Event: Give Rewards")
class ICANTCRY_API UEvent_GiveRewards : public UGameplayEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Rewards")
	float ExperienceToAdd = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Rewards")
	float HealthToRestore = 0.0f;

	// Sovrascriviamo la funzione base per metterci la logica delle stats
	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context) override;
	
};
