// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChallengeMinigame.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Actors/Pickups/RecipePickup.h"
#include "PuzzleAssembled.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UPuzzleAssembled : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget)) UTextBlock* Title;
	UPROPERTY(meta=(BindWidget)) UTextBlock* Body;
	UPROPERTY(meta=(BindWidget)) UButton* Return;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="Reward")
	ARecipePickup* Blueprint;

	UFUNCTION() void DisableMinigame();

	UPROPERTY() AChallengeMinigame* MinigameOwner;

public:
	void Display();
	void SetMinigameOwner(AChallengeMinigame* Minigame);
};
