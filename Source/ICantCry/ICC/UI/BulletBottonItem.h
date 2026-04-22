// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Actors/Bullet/Bullet.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "../Actors/Bullet/Bullet.h"
#include "../Actors/Bullet/BulletData.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "BulletBottonItem.generated.h"


class UInventoryHUD;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBulletBottonItem : public UUserWidget
{
	GENERATED_BODY()

public:
    void Setup(const FBullet& NewBullet, int32 InQuantity);
    void SetSelected(bool bIsSelected);
    void SetOwner(UInventoryHUD* Owner, int32 Index);
    void SetOwner(UInventoryHUD* Owner);

    bool IsHoverSelected() const;
    void SetIsSelected(const bool& Value);

    UImage* GetBulletIcon();
    UTextBlock* GetBulletName();
    UTextBlock* GetBulletQuantity();
    UButton* GetBulletButton();

    
    bool GetIsUnlocked() const;
 
    void SetIsUnlocked(const bool& Value);

    void SetUnlocked(bool bUnlocked);

    void Show();
    void Hide();

    FRecipe& GetBlueprint();
    FRecipe& GetSelectedRecipe();

protected:

    virtual void NativeConstruct() override;
    virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
    
    UPROPERTY(meta = (BindWidget)) UButton* SelectButton;
    UPROPERTY(meta = (BindWidget)) UBorder* SelectionBorder; // dave avere  IsVariable attivo

    
    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY(meta = (BindWidget)) UImage* BulletIconImage;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletQuantityText;

    UFUNCTION(BlueprintPure) FText RefreshQuantity();
    

private:

    void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);
    void UpdateQuantity();

    UPROPERTY()
    UInventoryHUD* OwnerHUD;

    int32 MyIndex; // Non credo ci serva piu 

    UPROPERTY()
    bool bIsHovered = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullets", meta=(AllowPrivateAccess=true))
    FBullet MyBullet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Bullets", meta=(AllowPrivateAccess=true))
    FRecipe BulletBlueprint;
    
    UFUNCTION() void DisplayBulletInfo();
    UFUNCTION() void HideBulletInfo();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullets", meta=(AllowPrivateAccess="true"))
	bool bIsUnlocked = false;
    
    
};
