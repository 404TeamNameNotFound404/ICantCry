// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractPromptWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UInteractPromptWidget::SetPromptData(FKey BoundKey, const FText& ActionText)
{
    if (ActionTextBlock)
    {
        ActionTextBlock->SetText(ActionText);
    }

    
    OnKeyPromptUpdated(BoundKey);
}

void UInteractPromptWidget::NativeConstruct()
{
    Super::NativeConstruct();
    bIsFocusable = false;
}


void UInteractPromptWidget::OnKeyPromptUpdated_Implementation(FKey BoundKey)
{
    if (!KeyImage) return;

    
    UTexture2D** FoundTexturePtr = KeyIconMap.Find(BoundKey);

    if (FoundTexturePtr && *FoundTexturePtr)
    {
       
        KeyImage->SetBrushFromTexture(*FoundTexturePtr);
        KeyImage->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        
        KeyImage->SetVisibility(ESlateVisibility::Collapsed);
    }
}