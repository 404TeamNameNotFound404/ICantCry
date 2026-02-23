// Fill out your copyright notice in the Description page of Project Settings.
#include "InGameMenu.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Inventory/CraftingTable.h"
#include "ICantCry/ICC/UI/InventoryHUD.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

void UInGameMenu::NativeConstruct()
{
	Super::NativeConstruct();
	Character->OnClicked.AddDynamic(this, &UInGameMenu::OpenCharacter);
	Inventory->OnClicked.AddDynamic(this, &UInGameMenu::OpenInventory);
	// Map->OnClicked.AddDynamic(this, &UInGameMenu::OpenMap);
	

	// For debugging purposes only!
	Map->SetIsEnabled(false);


	if (!InventoryHud)
	{
		InventoryHud = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDClass);
		InventoryHud->SetVisibility(ESlateVisibility::Hidden);
		InventoryHud->Setup();
	}

	
	if (!CharacterUI)
	{
		CharacterUI = CreateWidget<UCharacterUI>(GetWorld(), CharacterUIClass);
		CharacterUI->SetVisibility(ESlateVisibility::Visible);
		Main->AddChild(CharacterUI);
		CharacterUI->RefreshUI();
	}
}


/*
 * Il ragionamento di base è -> Io Apro il menu ---[Character]--------[Inventory]-------------[Map]---
 *                                                      |
 *                                                      |
 *                                           Default View---AddChild CharaUI <- Remove Child ->AddChild Inv
 *                                                      |         |
 *                                                      | -----< ---- - Click->     |
 *
 *    Disegno un po del cazzo ma in sostanza carichiamo tutto in native constructor poi premi C ,
 *    si apre il menu con character ui come window principale
 *    clicchi inventory, characterui viene rimossa da child e nascosta per aggiungere inventory e viceversa
 *    il crash del cazzo avveniva perche refresh veniva chiamato sul "else" quando ancora non stava in gioco la ui, refresh si fa una volta sola ogni volta che
 *    viene cliccato.
 */

void UInGameMenu::OpenInventory()
{
	
	if (CharacterUI)
	{
		Main->RemoveChild(CharacterUI);
	}
	
	Main->AddChild(InventoryHud);
	//InventoryHud->Setup();
	InventoryHud->Refresh();
	//CharacterUI->SetVisibility(ESlateVisibility::Hidden);
	InventoryHud->SetVisibility(ESlateVisibility::Visible);
}



void UInGameMenu::OpenCharacter()
{

	if (InventoryHud)
	{
		Main->RemoveChild(InventoryHud);
		InventoryHud->SetVisibility(ESlateVisibility::Hidden);
	}
	
	Main->AddChild(CharacterUI);
	CharacterUI->RefreshUI();
	//InventoryHud->SetVisibility(ESlateVisibility::Hidden);
	CharacterUI->SetVisibility(ESlateVisibility::Visible);

}



void UInGameMenu::OpenMap()
{
	
}

void UInGameMenu::InstantiateTable(AICC_Player* Player)
{
	Table = NewObject<UCraftingTable>(Player);
	checkf(Table, TEXT("Table not initialized"))
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
