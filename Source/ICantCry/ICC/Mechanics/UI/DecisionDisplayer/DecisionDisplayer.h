// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "DecisionDisplayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UDecisionDisplayer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget)) UTextBlock* DecisionText;

	UFUNCTION(BlueprintPure) FText UpdateDecisionText();

	UPROPERTY() FText M_DecisionText;

public:
	void SetDecisionText(const FText& Text);
	void Hide();
	void Show();
	void Show(bool& InPlayerTurn);
};
