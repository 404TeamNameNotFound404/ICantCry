// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Actors/Bullet/Casing.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "VendingMachineSlot.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UVendingMachineSlot : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget)) UButton* CasingSelection;
	UPROPERTY(meta = (BindWidget)) UButton* RecipeSelection;
	UPROPERTY(meta = (BindWidget)) UButton* EssenceSelection;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* CasingName;
	UPROPERTY(meta = (BindWidget)) UTextBlock* CasingQuantity;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* RecipeName;
	
	UPROPERTY(meta = (BindWidget)) UTextBlock* EssenceName;
	UPROPERTY(meta = (BindWidget)) UTextBlock* EssenceQuantity;
	
	UPROPERTY(meta = (BindWidget)) UButton* EssenceConfirm;
	UPROPERTY(meta = (BindWidget)) UButton* EssenceIncreaseQuantity;
	UPROPERTY(meta = (BindWidget)) UButton* EssenceDecreaseQuantity;
	
	
	UPROPERTY(meta = (BindWidget)) UButton* CasingConfirm;
	UPROPERTY(meta = (BindWidget)) UButton* CasingIncreaseQuantity;
	UPROPERTY(meta = (BindWidget)) UButton* CasingDecreaseQuantity;
	
	
	UPROPERTY(meta = (BindWidget)) UButton* RecipeConfirm;
	
	UPROPERTY() TArray<FRecipe> Recipes;
	UPROPERTY() TArray<FEssence> Essences;
	UPROPERTY() TArray<FCasing> Casings;
	
	
	UPROPERTY() FRecipe CurrentSelectedRecipe;
	UPROPERTY() FCasing CurrentSelectedCasing;
	UPROPERTY() FEssence CurrentSelectedEssence;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VendingMachine", meta=(AllowPrivateAccess = "true"))
	UDataTable* RecipeTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VendingMachine", meta=(AllowPrivateAccess = "true"))
	UDataTable* EssenceTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VendingMachine", meta=(AllowPrivateAccess = "true"))
	UDataTable* CasingTable;
	
	
	UFUNCTION(BlueprintPure) FText UpdateRecipeName();
	UFUNCTION(BlueprintPure) FText UpdateEssenceName();
	UFUNCTION(BlueprintPure) FText UpdateEssenceQuantity();
	UFUNCTION(BlueprintPure) FText UpdateCasingName();
	UFUNCTION(BlueprintPure) FText UpdateCasingQuantity();
	
	UPROPERTY() UICantCryGameInstance* Instance;
	
	
public:
	void GiveMe(const FEssence& What);
	void GiveMe(const FRecipe& What);
	void GiveMe(const FCasing What);
	
private:
	void Setup();
	void SetupCasings();
	void SetupRecipes();
	void SetupEssences();
	
	UFUNCTION() void ScrollRecipe();
	UFUNCTION() void ScrollEssence();
	UFUNCTION() void ScrollCasing();
	UFUNCTION() void AddRecipe();
	UFUNCTION() void IncreaseEssence();
	UFUNCTION() void DecreaseEssence();
	UFUNCTION() void AddEssence();
	UFUNCTION() void IncreaseCasingQuantity();
	UFUNCTION() void DecreaseCasingQuantity();
	UFUNCTION() void AddCasing();
	
	void RefreshRecipe();
	void RefreshEssence();
	
	void Refresh();
	
	UPROPERTY() int32 CurrentCasingIndex = 0;
	UPROPERTY() int32 CasingAmount = 0;
	UPROPERTY() int32 CurrentEssenceIndex = 0;
	UPROPERTY() int32 CurrentRecipeIndex = 0;
	//UPROPERTY() bool bGoldCasing;
};
