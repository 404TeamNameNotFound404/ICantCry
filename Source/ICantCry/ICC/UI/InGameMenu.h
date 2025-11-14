// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingHUD.h"
#include "BestiaryUI.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "InGameMenu.generated.h"


class AICC_Player;

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OpenOverview();
	
	UFUNCTION()
	void OpenCraft();

	UFUNCTION()
	void OpenInventory();

	// UFUNCTION()
	// void OpenBestiary();

	UFUNCTION()
	void OpenMap();

	void InstantiateTable(AICC_Player* Player);

	void SetMenuOpen(const bool& Value);
	bool IsOpen() const;

	void SetDisabled(const bool& Value);
	bool IsDisabled() const;


protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* Main;

	UPROPERTY(meta = (BindWidget))
	UButton* Overview;

	UPROPERTY(meta = (BindWidget))
	UButton* Craft;

	UPROPERTY(meta = (BindWidget))
	UButton* Inventory;

	UPROPERTY(meta = (BindWidget))
	UButton* Map;


private:

	UPROPERTY()
	UCraftingTable* Table;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventoryHUD> InventoryHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCraftingHUD> CraftingHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))  
    TSubclassOf<UBestiaryUI> BestiaryUIClass;
	
	UPROPERTY()
	UCraftingHUD* CraftingHud;

	UPROPERTY()
	UInventoryHUD* InventoryHud;

	UPROPERTY()
	UBestiaryUI* BestiaryUI;

	UPROPERTY()
	bool bMenuOpen = false;

	UPROPERTY()
	bool bDisable = false;
	
	// TODO Create UI Blueprint of Map and Overview
};
