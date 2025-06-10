// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingHUD.h"
#include "RecipeButtonWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"



void UCraftingHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (CraftButton)
    {
        CraftButton->OnClicked.AddDynamic(this, &UCraftingHUD::OnCraftClicked);
    }

    Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
    Player = Cast<AICC_Player>(Controller->GetPawn());
    
    // RecipeButtonWidget = CreateWidget<URecipeButtonWidget>(GetWorld(), RecipeButtonClass);
    
    RefreshUI();
    //RefreshRecipesList();
}

void UCraftingHUD::SetCraftingTable(UCraftingTable *InTable)
{
    CraftingTable = InTable;
}

void UCraftingHUD::RefreshUI()
{
    DebugHelper::LogWarning("UCraftingHUD::RefreshUI chiamato");

    if(!InventoryManager)
    {
        DebugHelper::LogError("InventoryManager INIT");
        InventoryManager = NewObject<UInventoryManager>(this);
    }

    if(!CraftingTable)
    {
        DebugHelper::LogError("CraftingTable INIT");
        CraftingTable = NewObject<UCraftingTable>(this);
        CraftingTable->Initialize();
    }
    
    checkf(CraftingTable, TEXT("Crafting table is null at CraftingHUD"))
    checkf(InventoryManager, TEXT("InventoryManager is null at CraftingHUD"))


    RecipeListScrollBar->ClearChildren();
    EssenceListScrollBar->ClearChildren();
    Inventory = Player->GetInventoryManager()->GetInventory();

    for (const FRecipe& R : Inventory.Recipes)
    {
        RecipeButtonWidget = CreateWidget<URecipeButtonWidget>(GetWorld(), RecipeButtonClass);
        //RecipeListContainer->AddChild(RecipeButtonWidget);
        RecipeListScrollBar->AddChild(RecipeButtonWidget);
        RecipeButtonWidget->SetPadding(FMargin(0, 3.5f));
        RecipeButtonWidget->Setup(R, this);
        RecipeListScrollBar->SetScrollBarVisibility(ESlateVisibility::Visible);
        RecipeListScrollBar->SetVisibility(ESlateVisibility::Visible);
        RecipeButtonWidget->SetVisibility(ESlateVisibility::Visible);
        DebugHelper::LogError("Add recipe for (const ERecipeType& Recipe : InventoryManager->GetInventory().OwnedBlueprints)");
    }

    DebugHelper::LogError("Inventory Essence size: " + FString::FromInt(Inventory.Essences.Num()));

    // --- MOSTRA TUTTE LE ESSENZE POSSEDUTE ---
    for (const FEssence& Essence : Inventory.Essences)
    {
        UEssenceWidget* EssenceWidget = CreateWidget<UEssenceWidget>(GetWorld(), EssenceWidgetClass);
        checkf(EssenceWidget, TEXT("Essence widget is null"))
        EssenceListScrollBar->AddChild(EssenceWidget);
        EssenceWidget->SetPadding(FMargin(0, 4.5f));
        EssenceWidget->Setup(Essence, Essence.Quantity);
        DebugHelper::LogError("Add essence");
    }

    // -- MOSTRA COUNT BOSSOLI --
    if (EmptyCasingCountText)
    {
        EmptyCasingCountText->SetText(FText::AsNumber(Inventory.EmptyCasingCount));
    }

    if (GoldCasingCountText)
    {
        GoldCasingCountText->SetText(FText::AsNumber(Inventory.GoldCasingCount));
    }

    // Se c'è una recipe già selezionata, aggiorna tutto 
    UpdateSelectedRecipeDetails();
  
}

void UCraftingHUD::RefreshRecipesList()
{
   

    // Mostra SOLO le ricette sbloccate
    // for (const FRecipe& Recipe : CraftingTable->GetAvailableRecipes())
    // {
    //    // AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());
    //
    //     URecipeButtonWidget* RecipeWid = CreateWidget<URecipeButtonWidget>(Controller, RecipeButtonClass);
    //     
    //     if (RecipeWid)
    //     {
    //         RecipeWid->Setup(Recipe, this);
    //         RecipeListContainer->AddChild(RecipeWid);
    //     }
    // }
}

void UCraftingHUD::UpdateEssenceList()
{
    for (const FEssence& Essence : Inventory.Essences)
    {
        if (Essence.Quantity <= 0) continue;

        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            // La quantità posseduta è la stessa di quella in Inventory
            Widget->Setup(Essence, Essence.Quantity);
        }
    }
}

void UCraftingHUD::UpdateSelectedRecipeDetails()
{
    if (!CraftingTable) 
    {
        return;
    }

    UBulletData* BulletData = SelectedRecipe.ResultBullet.GetBulletData();
    if (!BulletData) return;

    if (BulletImage && BulletData->Icon)
    {
        BulletImage->SetBrushFromTexture(BulletData->Icon);
    }

    if (BulletNameText)
    {
        BulletNameText->SetText(FText::FromString(BulletData->BulletName));
    }

    if (RecipeDescription)
    {
        FString MaterialReq;
        MaterialReq += FString::Printf(TEXT("Casing: %s x%d\n"),
            SelectedRecipe.RequiredCasingType == ECasingType::Base ? TEXT("Base") : TEXT("Gold"),
            SelectedRecipe.RequiredCasingQuantity);

        for (const FEssence& Ess : SelectedRecipe.RequiredEssences)
        {
            MaterialReq += FString::Printf(TEXT("Essence: %s x%d\n"),
                *UEnum::GetDisplayValueAsText(Ess.EssenceType).ToString(),
                Ess.Quantity);
        }

        RecipeDescription->SetText(FText::FromString(MaterialReq));
    }

    if (BulletEffectText)
    {
        BulletEffectText->SetText(FText::FromString(BulletData->Effect));
    }

    UpdateMaterialList();
    UpdateCraftButton();
}

void UCraftingHUD::UpdateMaterialList()
{

    for (const FEssence& Required : SelectedRecipe.RequiredEssences)
    {
        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            int32 OwnedQty = Inventory.GetEssenceQuantity(Required.EssenceType);
            Widget->Setup(Required, OwnedQty);
        }
    }
    
}

void UCraftingHUD::UpdateCraftButton()
{
    if (!CraftButton || !CraftingTable) 
    {
        return;
    }

    bool bCanCraft = CraftingTable->CanCraft();

    CraftButton->SetIsEnabled(bCanCraft);
    CraftButton->SetRenderOpacity(bCanCraft ? 1.f : 0.4f);
}

void UCraftingHUD::OnCraftClicked()
{
    if (CraftingTable)
    {
        CraftingTable->CraftBullet(SelectedRecipe.ResultBullet,
            SelectedRecipe.RequiredBlueprintType,
            SelectedRecipe.RequiredCasingType
        );

        RefreshUI(); 
    }
}

void UCraftingHUD::OnRecipeSelected( const FRecipe& NewRecipe)
{
    
    SelectedRecipe = NewRecipe;
    CraftingTable->SetRecipe(SelectedRecipe);
    CraftingTable->CheckResurces();

    UpdateSelectedRecipeDetails();
}

void UCraftingHUD::SetupCraftingHUD(UCraftingTable* InCraftingTable, UInventoryManager* InInventoryManager)
{
    CraftingTable = InCraftingTable;
    InventoryManager = InInventoryManager;
}

FInventory UCraftingHUD::GetInventory() const
{
    return Inventory;
}


