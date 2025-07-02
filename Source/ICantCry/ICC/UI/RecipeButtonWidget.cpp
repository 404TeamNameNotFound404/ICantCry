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

    Index = Recipe.Index;
    Name = Recipe.GetName(Recipe.RequiredBlueprintType);

    DebugHelper::LogMessage(5, FColor::FromHex("504B38"),  "Name and Index in widget " + Recipe.GetName(Recipe.RequiredBlueprintType) +  FString::FromInt(Index));

   
}


void URecipeButtonWidget::OnButtonClicked()
{
    Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    checkf(Instance, TEXT("Instance invalid"))
    checkf(OwnerHUD, TEXT("Owner hud invalid"))
    const FRecipe R = Instance->GetInventory().RecipeLists[Index];
    OwnerHUD->OnRecipeSelected(R);
}
