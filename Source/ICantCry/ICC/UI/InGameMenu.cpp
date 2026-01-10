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
		// Main->AddChild(InventoryHud);
		// InventoryHud->Refresh();
	}

	
	if (!CharacterUI)
	{
		CharacterUI = CreateWidget<UCharacterUI>(GetWorld(), CharacterUIClass);
		CharacterUI->SetVisibility(ESlateVisibility::Visible);
		Main->AddChild(CharacterUI);
		CharacterUI->RefreshUI();
	}
}


void UInGameMenu::OpenInventory()
{

	
	if (!InventoryHud)
	{
		InventoryHud = CreateWidget<UInventoryHUD>(GetWorld(), InventoryHUDClass);
		InventoryHud->SetVisibility(ESlateVisibility::Visible);
		Main->AddChild(InventoryHud);
		InventoryHud->Refresh();

	}
	else
	{
		Main->AddChild(InventoryHud);
		InventoryHud->Refresh();
		InventoryHud->SetVisibility(ESlateVisibility::Visible);
	}
	
	
}



void UInGameMenu::OpenCharacter()
{

	if (InventoryHud)
	{
		//Main->RemoveChild(InventoryHud);
		InventoryHud->SetVisibility(ESlateVisibility::Hidden);
	}

	
	// if (!CharacterUI)
	// {
	// 	CharacterUI = CreateWidget<UCharacterUI>(GetWorld(), CharacterUIClass);
	// 	CharacterUI->SetVisibility(ESlateVisibility::Visible);
	// 	Main->AddChild(CharacterUI);
	// 	CharacterUI->RefreshUI();
	// }
	
	Main->AddChild(CharacterUI);
	CharacterUI->RefreshUI();
	CharacterUI->SetVisibility(ESlateVisibility::Visible);

}



void UInGameMenu::OpenMap()
{
	
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
