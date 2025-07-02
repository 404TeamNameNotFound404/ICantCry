// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "RevolverSlot.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API URevolverSlot : public UUserWidget
{
	GENERATED_BODY()

public:

    UPROPERTY(meta = (BindWidget))
    UImage* BulletIcon;

    UPROPERTY(meta = (BindWidget))
    UImage* EmptySlotIcon;

    // Utility function if needed
    void SetFilled(bool bFilled, UTexture2D* BulletTexture = nullptr);
	
};
