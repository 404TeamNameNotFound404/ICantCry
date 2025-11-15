// Fill out your copyright notice in the Description page of Project Settings.


#include "EmotionButtonWidget.h"

void UEmotionButtonWidget::NativeConstruct()
{

    Super::NativeConstruct();

    // [DEBUG_START] - NativeConstruct
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] NativeConstruct - CurrentEmotion: %s, bIsLocked before: %s"), 
           *UEnum::GetValueAsString(CurrentEmotion),
           bIsLocked ? TEXT("YES") : TEXT("NO"));
    // [DEBUG_END]


    if (CurrentEmotion == EEmotionType::Default)
    {
        bIsLocked = true;
        bIsSelected = false;
    }

    // [DEBUG_START] - NativeConstruct after
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] NativeConstruct - bIsLocked after: %s"), 
           bIsLocked ? TEXT("YES") : TEXT("NO"));
    // [DEBUG_END]

    if(EmotionButton)
    {
        EmotionButton->OnClicked.AddDynamic(this, &UEmotionButtonWidget::OnButtonClicked);
    }

    if (SelectionHighlight)
    {
        SelectionHighlight->SetVisibility(ESlateVisibility::Collapsed);
    }

}

void UEmotionButtonWidget::SetupButton(EEmotionType EmotionType)
{

    // [DEBUG_START] - SetupButton start
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] SetupButton START for %s - bIsLocked before: %s"), 
           *UEnum::GetValueAsString(EmotionType),
           bIsLocked ? TEXT("YES") : TEXT("NO"));
    // [DEBUG_END]

    CurrentEmotion = EmotionType;

    bIsLocked = false;

    // [DEBUG_START] - SetupButton after unlock
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] SetupButton AFTER unlock for %s - bIsLocked: %s"), 
           *UEnum::GetValueAsString(CurrentEmotion),
           bIsLocked ? TEXT("YES") : TEXT("NO"));
    // [DEBUG_END]


   if (EmotionTxt)
    {
        if (const UEnum* EnumPtr = StaticEnum<EEmotionType>())
        {
            FText EmotionText = EnumPtr->GetDisplayNameTextByValue((int64)CurrentEmotion);
            EmotionTxt->SetText(EmotionText);
            UE_LOG(LogTemp, Warning, TEXT("[EmotionButton] Set text to: %s"), *EmotionText.ToString());
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[EmotionButton] EmotionTxt is NULL!"));
    }

    if (EmotionButton)
    {
        EmotionButton->SetIsEnabled(true);
    }


    // Aggiorna lo stato di selezione
    SetSelected(bIsSelected);

    if (SelectionHighlight)
    {
        SelectionHighlight->SetVisibility(bIsSelected ? 
            ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }

}


void UEmotionButtonWidget::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;

    if (SelectionHighlight)
    {
        SelectionHighlight->SetVisibility(bIsSelected ? 
            ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}


void UEmotionButtonWidget::SetLocked(bool bLocked)
{
    

    // [DEBUG_START] - SetLocked called
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] SetLocked CALLED for %s - from %s to %s"), 
           *UEnum::GetValueAsString(CurrentEmotion),
           bIsLocked ? TEXT("LOCKED") : TEXT("UNLOCKED"),
           bLocked ? TEXT("LOCKED") : TEXT("UNLOCKED"));
    // [DEBUG_END]

    bIsLocked = bLocked;

    if(EmotionTxt)
    {
        if (bIsLocked)
        {         
            EmotionTxt->SetText(FText::FromString(TEXT("???")));
        }
        else
        {
            if (const UEnum* EnumPtr = StaticEnum<EEmotionType>())
            {
                EmotionTxt->SetText(EnumPtr->GetDisplayNameTextByValue((int64)CurrentEmotion));
            }
        }
    }

    if (EmotionButton)
    {
        //EmotionButton->SetIsEnabled(!bIsLocked);
         EmotionButton->SetIsEnabled(true);
        // UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Button %s enabled: %s"), 
        //        *UEnum::GetValueAsString(CurrentEmotion),
        //        (!bIsLocked) ? TEXT("YES") : TEXT("NO"));
    }
}


void UEmotionButtonWidget::OnButtonClicked()
{

     // [DEBUG_START] - Button click debug
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] === EMOTION BUTTON CLICKED ==="));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Emotion: %s"), *UEnum::GetValueAsString(CurrentEmotion));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] bIsLocked: %s"), bIsLocked ? TEXT("YES") : TEXT("NO"));
    UE_LOG(LogTemp, Warning, TEXT("[DEBUG] EmotionButton enabled: %s"), EmotionButton->GetIsEnabled() ? TEXT("YES") : TEXT("NO"));
    // [DEBUG_END]

    // if (!bIsLocked)
    // {
            OnEmotionSelected.Broadcast(CurrentEmotion);
    //     UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Broadcasted emotion: %s"), *UEnum::GetValueAsString(CurrentEmotion));
    // }
    // else
    // {
    //     UE_LOG(LogTemp, Warning, TEXT("[DEBUG] Button is LOCKED - no broadcast"));
    // }

    
}
