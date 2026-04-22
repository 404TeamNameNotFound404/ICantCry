// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CraftingHUD.h"
#include "BestiaryUI.h"
#include "CharacterUI.h"
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
	void OpenInventory();

	UFUNCTION()
	void OpenCharacter();


	UFUNCTION()
	void OpenMap();

	void InstantiateTable(AICC_Player* Player);

	void SetMenuOpen(const bool& Value);
	bool IsOpen() const;

	void SetDisabled(const bool& Value);
	bool IsDisabled() const;
	
	UCharacterUI* GetCharacterUI();


protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	
	void LeftNavigation();
	void RightNavigation();
	void NavigateThroughtButtons(const int32& InDirection);
	void Bind();
	void PadClick();
	
	UPROPERTY(meta = (BindWidget))
	UScaleBox* Main;

	UPROPERTY(meta = (BindWidget))
	UButton* Inventory;

	UPROPERTY(meta = (BindWidget))
	UButton* Character;

	UPROPERTY(meta = (BindWidget))
	UButton* Map;
	
	UPROPERTY(meta=(BindWidget))
	UOverlay* OverviewFrame;
	
	void Hightlight(UWidget* What);
	
	UPROPERTY() TArray<UButton*> Scrollers;
	UPROPERTY() int32 ScrollerIndex;
	UPROPERTY() AICC_PlayerController* Controller;


private:

	UPROPERTY()
	UCraftingTable* Table;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UInventoryHUD> InventoryHUDClass;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCharacterUI> CharacterUIClass;
	
	UPROPERTY()
	UCraftingHUD* CraftingHud;

	UPROPERTY()
	UInventoryHUD* InventoryHud;

	UPROPERTY()
	UBestiaryUI* BestiaryUI;

	UPROPERTY()
	UCharacterUI* CharacterUI;

	UPROPERTY()
	bool bMenuOpen = false;

	UPROPERTY()
	bool bDisable = false;
	
	
};
