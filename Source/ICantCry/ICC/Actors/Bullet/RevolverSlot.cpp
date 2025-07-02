// Fill out your copyright notice in the Description page of Project Settings.


#include "RevolverSlot.h"


void URevolverSlot::SetFilled(bool bFilled, UTexture2D* BulletTexture)
{
    // if (bFilled && BulletTexture)
    // {
    //     BulletIcon->SetBrushFromTexture(BulletTexture);
    //     BulletIcon->SetVisibility(ESlateVisibility::Visible);
    //     EmptySlotIcon->SetVisibility(ESlateVisibility::Hidden);
    // }
    // else
    // {
    //     BulletIcon->SetVisibility(ESlateVisibility::Hidden);
    //     EmptySlotIcon->SetVisibility(ESlateVisibility::Visible);
    // }

    if (BulletIcon && EmptySlotIcon)
    {
        BulletIcon->SetVisibility(bFilled ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
        EmptySlotIcon->SetVisibility(bFilled ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
        
        if (bFilled && BulletTexture)
        {
            BulletIcon->SetBrushFromTexture(BulletTexture);
        }
    }

    
}

