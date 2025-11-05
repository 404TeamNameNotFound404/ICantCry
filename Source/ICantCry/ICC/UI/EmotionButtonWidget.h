// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Actors/EmotionsData/EmotionsData.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "EmotionButtonWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEmotionSelectedSignature, EEmotionType, Emotion);

UCLASS()
class ICANTCRY_API UEmotionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void SetupButton(EEmotionType EmotionType);

	UFUNCTION(BlueprintCallable) // highlight button if selected
	void SetSelected(bool bSelected);

	// lock or unlock the button (displays ??? if locked)
	UFUNCTION(BlueprintCallable)
	void SetLocked(bool bLocked);

	// event that is broadcast when this button is clicked
	UPROPERTY(BlueprintAssignable)
	FOnEmotionSelectedSignature OnEmotionSelected;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* EmotionButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EmotionTxt;

	UPROPERTY(meta = (BindWidget))
	UImage* SelectionHighlight;

private:
	UFUNCTION()
	void OnButtonClicked();

	EEmotionType CurrentEmotion; // the emotion that this button represents

	UPROPERTY()
	bool bIsLocked = true;

	UPROPERTY()
	bool bIsSelected = false;


};
