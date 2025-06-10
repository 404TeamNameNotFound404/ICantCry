// Fill out your copyright notice in the Description page of Project Settings.

#include "EssenceWidget.h"

void UEssenceWidget::Setup(const FEssence &Required, int32 OwnedQuantity)
{
    check(EssenceName)
    if (EssenceName)
    {
        EssenceName->SetText(FText::FromString(Required.GetName(Required.EssenceType)));
    }

    check(EssenceQuantity)
    
    if (EssenceQuantity)
    {
        FString QuantityStr = FString::FromInt(OwnedQuantity) + " / "+ FString::FromInt(Required.Quantity);
        EssenceQuantity->SetText(FText::FromString(QuantityStr));
    }
}