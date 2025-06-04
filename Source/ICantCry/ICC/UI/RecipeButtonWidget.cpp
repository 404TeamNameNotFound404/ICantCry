// Fill out your copyright notice in the Description page of Project Settings.

#include "RecipeButtonWidget.h"
#include "../UI/CraftingHUD.h"


void URecipeButtonWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (SelectButton)
    {
        SelectButton->OnClicked.AddDynamic(this, &URecipeButtonWidget::OnButtonClicked);
    }
}

void URecipeButtonWidget::Setup( const FRecipe& InRecipe, UCraftingHUD* InOwnerHUD)
{
    LinkedRecipe = InRecipe;
    OwnerHUD = InOwnerHUD;

    check(RecipeNameText)
    
    if (RecipeNameText)
    {
        RecipeNameText->SetText(FText::FromString(LinkedRecipe.ResultBullet.GetBulletData()->BulletName));
    }
}



void URecipeButtonWidget::OnButtonClicked()
{
    if (OwnerHUD)
    {
        const FRecipe R = LinkedRecipe;
        OwnerHUD->OnRecipeSelected(R);
    }
}