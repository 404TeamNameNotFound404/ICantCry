// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "StatsButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UStatsButtonWidget : public UUserWidget
{
	GENERATED_BODY()


public:

 	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStatsButtonClicked);

	virtual void NativeConstruct() override;

	
    UPROPERTY(BlueprintAssignable, Category = "StatsButton")FOnStatsButtonClicked OnStatsButtonClicked;

	UFUNCTION(BlueprintCallable, Category = "Stats Button") void UpdateStatValue(int32 Value);
	UFUNCTION(BlueprintCallable, Category = "Stats Button") void SetStatName(FString Name);
	UFUNCTION(BlueprintCallable, Category = "Stats Button") void SetSelected(bool bSelected);
    UFUNCTION(BlueprintCallable, Category = "Stats Button") bool IsSelected() const;
    
protected:

	UPROPERTY(meta = (BindWidget)) UButton* StatButton;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StatNameTxt;
	UPROPERTY(meta = (BindWidget)) UTextBlock* StatValueTxt;
	UPROPERTY(meta = (BindWidget)) UImage* SelectionHighlight;
	
private: 

 	UPROPERTY() bool bIsSelected = false;
	UFUNCTION() void OnButtonClicked();
	
};
