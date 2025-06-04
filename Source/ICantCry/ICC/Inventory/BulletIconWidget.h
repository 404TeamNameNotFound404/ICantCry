// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "BulletIconWidget.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UBulletIconWidget : public UUserWidget
{
	GENERATED_BODY()

    // CLASS TO SHOW BULLET ICON IN BATTLE HUD
public:

    UFUNCTION(BlueprintCallable)
    void SetIcon(UTexture2D* Texture);

    UFUNCTION(BlueprintCallable)
    void SetQuantity(int32 Quantity);

private:
    UPROPERTY(meta = (BindWidget))
    class UImage* IconImage;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* QuantityText;
	
};
