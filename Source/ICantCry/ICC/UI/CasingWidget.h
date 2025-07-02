// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "CasingWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UCasingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UImage* Case;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CaseName;

public:
	UImage* GetCasingImage();

	UTextBlock* GetCaseName();
};
