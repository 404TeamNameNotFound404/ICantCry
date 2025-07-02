// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "../Source/ICantCry/ICC/Actors/Bullet/Essence.h"
#include "EssenceWidget.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UEssenceWidget : public UUserWidget
{
	GENERATED_BODY()

public:

    void Setup(const FEssence& Required, int32 OwnedQuantity);
	
	
protected:
    //UPROPERTY(meta = (BindWidget)) UImage* EssenceIcon;
    UPROPERTY(meta = (BindWidget)) UTextBlock* EssenceName;
    UPROPERTY(meta = (BindWidget)) UTextBlock* EssenceQuantity;
	
};
