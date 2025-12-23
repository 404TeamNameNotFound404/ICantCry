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

	UFUNCTION() void DisableMinigame();

	UPROPERTY() AChallengeMinigame* MinigameOwner;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
	ERecipeType RecipeType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reward")
	FRecipe Self;

public:
	void Display();
	void SetMinigameOwner(AChallengeMinigame* Minigame);
};
