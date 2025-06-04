// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "../Inventory/Recipe.h"
#include "RecipeButtonWidget.generated.h"


class UCraftingHUD;

/**
 * 
 */
UCLASS()
class ICANTCRY_API URecipeButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    void Setup( const FRecipe& InRecipe, UCraftingHUD* InOwnerHUD);

protected:

    virtual void NativeConstruct() override;

    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY(meta = (BindWidget)) UButton* SelectButton;
    UPROPERTY(meta = (BindWidget)) UTextBlock* RecipeNameText;

private:

    FRecipe LinkedRecipe;
    UCraftingHUD* OwnerHUD;
	
};
