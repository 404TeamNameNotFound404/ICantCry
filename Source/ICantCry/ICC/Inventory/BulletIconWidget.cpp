// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletIconWidget.h"

void UBulletIconWidget::SetIcon(UTexture2D* Texture)
{
    if (IconImage && Texture)
    {
        // Imposta direttamente la texture sull'immagine
        IconImage->SetBrushFromTexture(Texture);
    }
}

void UBulletIconWidget::SetQuantity(int32 Quantity)
{
    if (QuantityText)
    {
        QuantityText->SetText(FText::AsNumber(Quantity));
    }
}