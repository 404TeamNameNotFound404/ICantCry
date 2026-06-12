// Fill out your copyright notice in the Description page of Project Settings.


#include "DistantCircleWidget.h"
#include "Components/Image.h"

void UDistantCircleWidget::SetIconActive(bool bIsActive)
{
    if (CircleImage)
    {
        // La logica di comparsa/scomparsa del singolo elemento grafico è gestita qui
        ESlateVisibility NewVisibility = bIsActive ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
        CircleImage->SetVisibility(NewVisibility);
    }
}

void UDistantCircleWidget::NativeConstruct()
{
    Super::NativeConstruct();
    bIsFocusable = false;
}
