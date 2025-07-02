// Fill out your copyright notice in the Description page of Project Settings.

#include "CraftingHUD.h"
#include "RecipeButtonWidget.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "../Source/ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Actors/Pickups/RecipePickup.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

FRecipe UCraftingHUD::SelectedRecipe;

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

    UCanvasPanelSlot* ImageSlot = Cast<UCanvasPanelSlot>(BulletImage->Slot);
    ImageSlot->SetZOrder(0);

    BulletImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.1f, 0.5f));
    
    UCanvasPanelSlot* DecriptionText = Cast<UCanvasPanelSlot>(RecipeDescription->Slot);
    DecriptionText->SetZOrder(1);

    RecipeDescription->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));


    BulletNameText->SetVisibility(ESlateVisibility::Hidden);
    RecipeDescription->SetVisibility(ESlateVisibility::Hidden);
    BulletEffectText->SetVisibility(ESlateVisibility::Hidden);

    
    CraftingTable = NewObject<UCraftingTable>(this);
    CraftingTable->Initialize();

    CraftButton->SetIsEnabled(false);
    
    RefreshUI();
    //RefreshRecipesList();
}

void UCraftingHUD::SetCraftingTable(UCraftingTable *InTable)
{
    CraftingTable = InTable;
}

void UCraftingHUD::RefreshUI()
{

    if(!InventoryManager)
    {
        InventoryManager = NewObject<UInventoryManager>(this);
    }

    CraftingTable->Initialize();
    
    checkf(CraftingTable, TEXT("Crafting table is null at CraftingHUD"))
    checkf(InventoryManager, TEXT("InventoryManager is null at CraftingHUD"))


   // checkf(RecipeListContainer, TEXT("RecipeContainer is null at CraftingHud"))
    //checkf(EssenceList, TEXT("EssenceList is null at CraftingHud"))
    
   // RecipeListContainer->ClearChildren();
   // EssenceList->ClearChildren();
    RecipeListScrollBar->ClearChildren();
    EssenceListScrollBar->ClearChildren();
    UICantCryGameInstance* GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    Inventory = GameInstance->GetInventory();

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
        TestIndex = R.Index; // TO DELETE
    }
    

    for (const auto& Essence : Inventory.EssencesStored)
    {
        UEssenceWidget* EssenceWidget = CreateWidget<UEssenceWidget>(GetWorld(), EssenceWidgetClass);
        checkf(EssenceWidget, TEXT("Essence widget is null"))

        EssenceListScrollBar->AddChild(EssenceWidget);
        const FEssence& E = Essence.Value;
        EssenceWidget->Setup(E, E.Quantity);
    }


    for (const auto& Pair : Inventory.CasingsStored)
    {
        const auto& Casing = Pair.Value;
    
        UCasingWidget* CasingSlot = CreateWidget<UCasingWidget>(GetWorld(), CasingWidgetClass);
        CasingSlot->GetCasingImage()->SetBrushFromTexture(Casing.GetIcon());
        FString Label = Casing.GetName() + " " + FString::FromInt(Casing.GetQuantity()) + "x";
        CasingSlot->GetCaseName()->SetText(FText::FromString(Label));
        CasingSlot->SetPadding(FMargin{0, 2.5f});
    }

    
    UpdateCasingTexts();

    UpdateSelectedRecipeDetails();
}

void UCraftingHUD::RefreshRecipesList()
{
    if (!CraftingTable) return;

    //RecipeListContainer->ClearChildren();

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
   if (!CraftingTable)
        return;

   // EssenceList->ClearChildren();

    
    
    for (const FEssence& Essence : Inventory.Essences)
    {
        if (Essence.Quantity <= 0) continue;

        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            // La quantità posseduta è la stessa di quella in Inventory
            Widget->Setup(Essence, Essence.Quantity);
            //EssenceList->AddChild(Widget);
            EssenceListScrollBar->AddChild(Widget);
        }
    }
}

void UCraftingHUD::UpdateSelectedRecipeDetails()
{
    if (!CraftingTable) 
    {
        return;
    }

    UICantCryGameInstance* GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    
    const UBulletData* BulletData = GameInstance->GetInventory().GetSelectedRecipe().ResultBullet.GetBulletData();

    if (!BulletData)
    {
        return;
    }

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

   // UpdateMaterialList();
    //UpdateCraftButton();
}

void UCraftingHUD::UpdateMaterialList()
{
   if (!CraftingTable) return;

    //EssenceList->ClearChildren();
    

    for (const FEssence& Required : SelectedRecipe.RequiredEssences)
    {
        UEssenceWidget* Widget = CreateWidget<UEssenceWidget>(this, EssenceWidgetClass);
        if (Widget)
        {
            int32 OwnedQty = Inventory.GetEssenceQuantity(Required.EssenceType);
            Widget->Setup(Required, OwnedQty);
            EssenceListScrollBar->AddChild(Widget);
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
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (CraftingTable)
    {
        CraftingTable->CraftBullet(Instance->GetInventory().GetSelectedRecipe().ResultBullet,
            Instance->GetInventory().GetSelectedRecipe().RequiredBlueprintType,
            Instance->GetInventory().GetSelectedRecipe().RequiredCasingType
        );

        RefreshUI(); 
    }
}

void UCraftingHUD::UpdateCasingTexts()
{
    for (auto& Casing : Inventory.CasingsStored)
    {
        FCasing C = Casing.Value;

       switch (C.GetType())
       {
       case ECasingType::Base:
           EmptyCasingCountText->SetText(FText::FromString(FString::FromInt(C.GetQuantity())));
           CasingImage->SetBrushFromTexture(C.GetIcon());
           break;
       case ECasingType::Gold:
           GoldCasingCountText->SetText(FText::FromString(FString::FromInt(C.GetQuantity())));
           GoldCasingImage->SetBrushFromTexture(C.GetIcon());
           break;
       }
    }
}

void UCraftingHUD::OnRecipeSelected( const FRecipe& NewRecipe)
{
    UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    checkf(Instance, TEXT("Instance is null"))
    Instance->GetInventory().SetSelectedRecipe(NewRecipe);
    
    SelectedRecipe = NewRecipe;
    
    
    DisplayInfo(SelectedRecipe);
    UpdateSelectedRecipeDetails();

    CraftingTable->ScanResources();

    CraftButton->SetIsEnabled(false);
    
    if (CraftingTable->CanCraft())
    {
        CraftButton->SetIsEnabled(true);
    }
    
}

void UCraftingHUD::SetupCraftingHUD(UCraftingTable* InCraftingTable, UInventoryManager* InInventoryManager)
{
    CraftingTable = InCraftingTable;
    InventoryManager = InInventoryManager;
}

void UCraftingHUD::ClearInfo(const FRecipe& NewRecipe)
{
    SelectedRecipe.Description = FText::FromString("");
    BulletNameText->SetText(FText::FromString(""));
    RecipeDescription->SetText(SelectedRecipe.Description);
}

void UCraftingHUD::DisplayInfo(const FRecipe& NewRecipe)
{
    const FText BaseDescription = FText::FromString("Type: ");
    const FText RecipeName = FText::FromString(NewRecipe.GetName(NewRecipe.RequiredBlueprintType));
    const FText EffectDescription = FText::FromString("Effect: ");
    BulletNameText->SetText(FText::Format(FText::FromString("{0} {1}"), BaseDescription, RecipeName));
    RecipeDescription->SetText(NewRecipe.DisplayDescription());
    BulletEffectText->SetText(EffectDescription);

    BulletNameText->SetVisibility(ESlateVisibility::Visible);
    RecipeDescription->SetVisibility(ESlateVisibility::Visible);
    BulletEffectText->SetVisibility(ESlateVisibility::Visible);
}


FInventory UCraftingHUD::GetInventory() const
{
    return Inventory;
}


