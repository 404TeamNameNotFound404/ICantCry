// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DistantCircleWidget.generated.h"


class UImage;

/**
* CLASS: UDistantCircleWidget
* Widget displayed when the player is at medium distance from the interactable object.
*/
UCLASS()
class ICANTCRY_API UDistantCircleWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    /** Manages icon visibility and options */
    void SetIconActive(bool bIsActive);

protected:

    virtual void NativeConstruct() override;
    
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> CircleImage;

    bool bIsFocusable;
    
	
};
