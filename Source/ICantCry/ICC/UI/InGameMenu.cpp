// Fill out your copyright notice in the Description page of Project Settings.
#include "InGameMenu.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Inventory/CraftingTable.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();

	
	// Overview->OnClicked.AddDynamic(this, &UInGameMenu::OpenOverview);
	Craft->OnClicked.AddDynamic(this, &UInGameMenu::OpenCraft);
	Inventory->OnClicked.AddDynamic(this, &UInGameMenu::OpenInventory);
	// Map->OnClicked.AddDynamic(this, &UInGameMenu::OpenMap);
	//Bestiary->OnClicked.AddDynamic(this, &UInGameMenu::OpenBestiary);

	// For debugging purposes only!
	Map->SetIsEnabled(false);
	Overview->SetIsEnabled(false);
}


void UInGameMenu::OpenOverview()
{
	
	if (CraftingHud)
	{
		CraftingHud->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameMenu::OpenCraft()
{

	// if (BestiaryUI)
    // {
    //     Main->RemoveChild(BestiaryUI);
    //     BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
    // }

	if (InventoryHud)
	{
		Main->RemoveChild(InventoryHud);
	}
	
	if (!CraftingHud)
	{
		CraftingHud = CreateWidget<UCraftingHUD>(GetWorld(), CraftingHUDClass);
		CraftingHud->SetCraftingTable(Table);
		Main->AddChild(CraftingHud);
		CraftingHud->RefreshUI();
	}
	
	Main->AddChild(CraftingHud);
	CraftingHud->RefreshUI();
	CraftingHud->SetVisibility(ESlateVisibility::Visible);

}

void UInGameMenu::OpenInventory()
{


	// if (BestiaryUI)
    // {
    //     Main->RemoveChild(BestiaryUI);
    //     BestiaryUI->SetVisibility(ESlateVisibility::Hidden);
    // }

	if (CraftingHud)
	{
		Main->RemoveChild(CraftingHud);
	}

	if (!InventoryHud)
	{
		InventoryHud = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDClass);
		InventoryHud->SetVisibility(ESlateVisibility::Visible);
		Main->AddChild(InventoryHud);
		InventoryHud->Refresh();
	}
	
	Main->AddChild(InventoryHud);
	InventoryHud->Refresh();
	InventoryHud->SetVisibility(ESlateVisibility::Visible);
}

// void UInGameMenu::OpenBestiary()
// {
// 	if (CraftingHud)
//     {
//         Main->RemoveChild(CraftingHud);
//         CraftingHud->SetVisibility(ESlateVisibility::Hidden);
//     }

//     if (InventoryHud)
//     {
//         Main->RemoveChild(InventoryHud);
//         InventoryHud->SetVisibility(ESlateVisibility::Hidden);
//     }

// 	if(!BestiaryUI)
// 	{
// 		if(BestiaryUIClass)
// 		{
// 			BestiaryUI = CreateWidget<UBestiaryUI>(GetWorld(), BestiaryUIClass);
// 			if (BestiaryUI)
//             {
//                 // BestiaryUI->SetupEmotionsData(EmotionsData);
//             	// BestiaryUI->SetupNoteData(NoteData);

//                 Main->AddChild(BestiaryUI);  	
// 				DebugHelper::LogError("UInGameMenu::OpenBestiary -> BestiaryUI created successfully");
//             }
//             else
//             {
// 				DebugHelper::LogError("UInGameMenu::OpenBestiary -> Failed to create BestiaryUI widget");
//                 return;
//             }
//         }
//         else
//         {

// 			DebugHelper::LogError("UInGameMenu::OpenBestiary -> BestiaryUIClass not set in InGameMenu");
//             return;
//         }
		
// 	}

// 	if (!BestiaryUI->IsInViewport())
//     {
//         Main->AddChild(BestiaryUI);
//     }
    
//     BestiaryUI->SetVisibility(ESlateVisibility::Visible);
// 	DebugHelper::LogError("UInGameMenu::OpenBestiary -> Bestiary opened");
    

	
   
// }

void UInGameMenu::OpenMap()
{
	if (CraftingHud)
	{
		CraftingHud->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInGameMenu::InstantiateTable(AICC_Player* Player)
{
	Table = NewObject<UCraftingTable>(Player);
	checkf(Table, TEXT("Table not initialized"))
	DebugHelper::LogSuccess("Table instantiated via ingamemenu");
}

void UInGameMenu::SetMenuOpen(const bool& Value)
{
	bMenuOpen = Value;
}

bool UInGameMenu::IsOpen() const
{
	return bMenuOpen;
}

void UInGameMenu::SetDisabled(const bool& Value)
{
	bDisable = Value;
}

bool UInGameMenu::IsDisabled() const
{
	return bDisable;
}
