// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_PrepareDelivery.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"

void UEvent_PrepareDelivery::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    if (UDialogueWidget* Widget = Cast<UDialogueWidget>(Context))
    {
        Widget->CurrentRequiredItemTag = ItemTag;
        Widget->CurrentAmountRequired = Amount;
        Widget->CurrentQuestTag = QuestTag;
        Widget->CurrentObjectiveTag = ObjectiveTag;
        Widget->UpdateDeliveryUI();
    }
}