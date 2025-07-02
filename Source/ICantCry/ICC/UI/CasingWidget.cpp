// Fill out your copyright notice in the Description page of Project Settings.


#include "CasingWidget.h"

void UCasingWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

UImage* UCasingWidget::GetCasingImage()
{
	return Case;
}

UTextBlock* UCasingWidget::GetCaseName()
{
	return CaseName;
}
