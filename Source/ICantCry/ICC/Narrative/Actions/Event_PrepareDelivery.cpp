// Fill out your copyright notice in the Description page of Project Settings.


#include "Event_PrepareDelivery.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"



void UEvent_PrepareDelivery::ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context)
{
    UDialogueWidget* Widget = Cast<UDialogueWidget>(Context);
    if (!Widget)
    {
        // this event only makes sense inside a dialogue line: anywhere else there is no Deliver button
        UE_LOG(LogTemp, Error, TEXT("Event_PrepareDelivery: il Context non e' un DialogueWidget (%s). Mettilo negli Events di una linea di dialogo."),
            Context ? *Context->GetName() : TEXT("NULL"));
        return;
    }

    if (!ItemTag.IsValid() || !QuestTag.IsValid() || !ObjectiveTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("Event_PrepareDelivery: ItemTag, QuestTag o ObjectiveTag vuoti in %s, consegna non aperta."),
            *GetOuter()->GetName());
        return;
    }

    Widget->CurrentRequiredItemTag = ItemTag;
    Widget->CurrentAmountRequired = Amount;
    Widget->CurrentQuestTag = QuestTag;
    Widget->CurrentObjectiveTag = ObjectiveTag;
    Widget->CurrentProgressTextFormat = ProgressText;
    Widget->UpdateDeliveryUI();
}