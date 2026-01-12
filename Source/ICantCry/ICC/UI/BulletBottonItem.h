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

    bool IsHoverSelected() const;
    void SetIsSelected(const bool& Value);

    UImage* GetBulletIcon();
    UTextBlock* GetBulletName();
    UTextBlock* GetBulletQuantity();
    UButton* GetBulletButton();

protected:

    virtual void NativeConstruct() override;
    
    UPROPERTY(meta = (BindWidget)) UButton* SelectButton;
    UPROPERTY(meta = (BindWidget)) UBorder* SelectionBorder; // dave avere  IsVariable attivo

    
    UFUNCTION()
    void OnButtonClicked();

    UPROPERTY(meta = (BindWidget)) UImage* BulletIconImage;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletNameText;
    UPROPERTY(meta = (BindWidget)) UTextBlock* BulletQuantityText;
    

private:

    void SetBrushFromTexture(UTexture2D* Texture, bool bMatchSize = false);

    UPROPERTY()
    UInventoryHUD* OwnerHUD;

    int32 MyIndex;

    UPROPERTY()
    bool bIsHovered = false;

    UPROPERTY()
    FBullet MyBullet;;
    
    UFUNCTION() void DisplayBulletInfo();
    UFUNCTION() void HideBulletInfo();
    
};
