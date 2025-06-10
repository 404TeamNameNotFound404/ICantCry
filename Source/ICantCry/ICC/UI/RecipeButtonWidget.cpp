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

void URecipeButtonWidget::Setup( const ERecipeType& InRecipe, UCraftingHUD* InOwnerHUD)
{
    checkf(InOwnerHUD, TEXT("InOwnerHUD null at URecipeButtonWidget::Setup"))
    
    //LinkedRecipe = InRecipe;
    OwnerHUD = InOwnerHUD;

    checkf(RecipeNameText, TEXT("RecipeName null at URecipeButtonWidget::Setup"))
    RecipeNameText->SetText(FText::FromString(/*LinkedRecipe.ResultBullet.GetBulletData()->BulletName*/ "Test 1 2 3"));
    DebugHelper::LogError("Setup Recipe called");
}

void URecipeButtonWidget::Setup(const FRecipe& Recipe, UCraftingHUD* InOwnerHUD)
{
    checkf(InOwnerHUD, TEXT("InOwnerHUD null at URecipeButtonWidget::Setup"))
    OwnerHUD = InOwnerHUD;
    checkf(RecipeNameText, TEXT("RecipeName null at URecipeButtonWidget::Setup"))
    RecipeNameText->SetText(FText::FromString(Recipe.GetName(Recipe.RequiredBlueprintType)));
    DebugHelper::LogError("Setup Recipe called");
}


void URecipeButtonWidget::OnButtonClicked()
{
    if (OwnerHUD)
    {
        const FRecipe R = LinkedRecipe;
        OwnerHUD->OnRecipeSelected(R);
    }
}
