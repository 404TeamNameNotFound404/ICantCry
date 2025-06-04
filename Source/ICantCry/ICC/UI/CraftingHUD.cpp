// Fill out your copyright notice in the Description page of Project Settings.


#include "CraftingHUD.h"
#include "RecipeButtonWidget.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"



void UCraftingHUD::NativeConstruct()
{
    Super::NativeConstruct();

    if (CraftButton)
    {
        CraftButton->OnClicked.AddDynamic(this, &UCraftingHUD::OnCraftClicked);
    }

    //Controller = AICC_PlayerController::GetICCPlayerController(this);
    
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
    
    check(CraftingTable)
    check(InventoryManager)
    // if (!CraftingTable || !CraftingTable->GetInventory()|| !InventoryManager) 
    // {

    //     return;
    //    // UE_LOG(LogTemp, Warning, TEXT("Controllo blueprint: %d - Posseduta? %s"), (int32)Recipe.RequiredBlueprintType, Inventory->HasBlueprint(Recipe.RequiredBlueprintType) ? TEXT("SI") : TEXT("NO"));
    // }


    check(RecipeListContainer)
    check(EssenceList)

    // if (RecipeListContainer) 
    // {
         RecipeListContainer->ClearChildren();
    // }

    // if (EssenceList) 
    // {
        EssenceList->ClearChildren();
    // }

    FInventory Inventory = CraftingTable->GetInventory();

    // -- POPOLA LISTA DELLE BLUEPRINTS --
    for ( const FRecipe& Recipe : CraftingTable->GetAvailableRecipes())
    {
        if (!Inventory.HasBlueprint(Recipe.RequiredBlueprintType)) 
        {
            DebugHelper::LogError("SKIPPAAA");
            continue;
        }

        AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

        URecipeButtonWidget* RecipeWidget = CreateWidget<URecipeButtonWidget>(Controller, RecipeButtonClass); 

        checkf(RecipeWidget, TEXT("RecipeWidget è nullo "))
        
        
            
        RecipeListContainer->AddChild(RecipeWidget);
        RecipeWidget->Setup(Recipe, this);
        RecipeWidget->SetVisibility(ESlateVisibility::Visible);
        DebugHelper::LogError("Add recipe");
       
    }

    // --- MOSTRA TUTTE LE ESSENZE POSSEDUTE ---
    for (const FEssence& Essence : Inventory.Essences)
    {
        // UUserWidget* Widget = CreateWidget<UUserWidget>(this, EssenceWidgetClass);

        // UEssenceWidget* EssenceWidget = Cast<UEssenceWidget>(Widget);

        AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

        UEssenceWidget* EssenceWidget = CreateWidget<UEssenceWidget>(Controller, EssenceWidgetClass);

        check(EssenceWidget)
        
        EssenceList->AddChild(EssenceWidget);
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
    if (!RecipeListContainer || !CraftingTable) return;

    RecipeListContainer->ClearChildren();

    // Mostra SOLO le ricette sbloccate
    for (const FRecipe& Recipe : CraftingTable->GetAvailableRecipes())
    {
        AICC_PlayerController* Controller = Cast<AICC_PlayerController>(GetWorld()->GetFirstPlayerController());

        URecipeButtonWidget* RecipeWid = CreateWidget<URecipeButtonWidget>(Controller, RecipeButtonClass);
        if (RecipeWid)
        {
            RecipeWid->Setup(Recipe, this);
            RecipeListContainer->AddChild(RecipeWid);
        }
    }
}

void UCraftingHUD::UpdateEssenceList()
{
   if (!CraftingTable || !EssenceList)
        return;

    EssenceList->ClearChildren();

    const FInventory& Inventory = CraftingTable->GetInventory();
    
    for (const FEssence& Essence : Inventory.Essences)
    {
        if (Essence.Quantity <= 0) continue;

        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            // La quantità posseduta è la stessa di quella in Inventory
            Widget->Setup(Essence, Essence.Quantity);
            EssenceList->AddChild(Widget);
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
   if (!EssenceList || !CraftingTable) return;

    EssenceList->ClearChildren();
    FInventory Inventory = CraftingTable->GetInventory();

    for (const FEssence& Required : SelectedRecipe.RequiredEssences)
    {
        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            int32 OwnedQty = Inventory.GetEssenceQuantity(Required.EssenceType);
            Widget->Setup(Required, OwnedQty);
            EssenceList->AddChild(Widget);
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


