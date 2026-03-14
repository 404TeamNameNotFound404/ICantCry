// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"

UInteractionComponent::UInteractionComponent() 
{ 
	PrimaryComponentTick.bCanEverTick = false; 
}


void UInteractionComponent::TriggerInteraction(AICC_Player* Player)
{
	// if (!Player || !DialogueWidgetClass) return;

    // UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    // UDialogueAsset* DialogueToPlay = FinalDefaultDialogue;
    // bool bOptional = false;

    // if (QM)
    // {
    //     for (const FQuestDialogueChain& Step : QuestChain)
    //     {
    //         if (!Step.Quest) continue;
    //         FGameplayTag QID = Step.Quest->QuestID;

    //         // 1. Se la missione è DEFINITIVAMENTE completata (premio già riscosso)
    //         if (QM->IsQuestCompleted(QID))
    //         {
    //             DialogueToPlay = Step.CompletedDialogue; 
    //             continue; // Cerchiamo se c'è una missione successiva
    //         }

    //         // 2. Se la missione è attiva, controlliamo se è PRONTA per essere consegnata
    //        if (QM->IsQuestActive(QID))
    //         {
    //             // CAMBIO QUI: Usiamo la nuova funzione ByTag
    //             if (QM->AreObjectivesCompleteByTag(QID)) 
    //             {
    //                 DialogueToPlay = Step.CompletedDialogue;
    //             }
    //             else 
    //             {
    //                 DialogueToPlay = Step.InProgressDialogue;
    //             }
                
    //             bOptional = false; 
    //             break;
    //         }

    //         // 3. Prima missione non iniziata
    //         DialogueToPlay = Step.StartDialogue;
    //         bOptional = Step.bIsOptional;
    //         break;
    //     }
    // }

    // if (DialogueToPlay)
    // {
    //     UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
    //     if (DialogueWidget)
    //     {
    //         DialogueWidget->AddToViewport();
    //         DialogueWidget->SetIsOptional(bOptional); // Nuova funzione nel widget
    //         DialogueWidget->StartDialogue(DialogueToPlay);
    //     }
    // }


    UQuestManagerSystem* QM = Player->GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    
    // Iniziamo con NULL invece che con il default.
    // Se alla fine del ciclo DialogueToPlay è ancora NULL, l'NPC non farà nulla.
    UDialogueAsset* DialogueToPlay = nullptr; 
    bool bOptional = false;

    if (QM)
    {
        bool bAllQuestsFinished = true;

        for (const FQuestDialogueChain& Step : QuestChain)
        {
            if (!Step.Quest) continue;
            FGameplayTag QID = Step.Quest->QuestID;

            // Se la missione è già stata completata e consegnata, 
            // la saltiamo e passiamo alla prossima della catena.
            if (QM->IsQuestCompleted(QID))
            {
                continue; 
            }

            // Se arriviamo qui, abbiamo trovato la missione "corrente" (Attiva o Nuova)
            bAllQuestsFinished = false;

            if (QM->IsQuestActive(QID))
            {
                if (QM->AreObjectivesCompleteByTag(QID)) 
                {
                    DialogueToPlay = Step.CompletedDialogue;
                }
                else 
                {
                    DialogueToPlay = Step.InProgressDialogue;
                }
            }
            else
            {
                // È una missione nuova (Start)
                DialogueToPlay = Step.StartDialogue;
                bOptional = Step.bIsOptional;
            }

            // Trovata la missione corrente, interrompiamo il ciclo.
            break; 
        }

        // Se il ciclo è finito e tutte le missioni sono completate...
        if (bAllQuestsFinished)
        {
            // ...usiamo il dialogo finale (quello "stupido" dei designer)
            DialogueToPlay = FinalDefaultDialogue;
        }
    }

    // Se DialogueToPlay è valido, mostriamo il widget. 
    // Se è NULL (perché FinalDefaultDialogue è vuoto), il player preme il tasto ma non succede nulla.
    if (DialogueToPlay)
    {
        UDialogueWidget* DialogueWidget = CreateWidget<UDialogueWidget>(GetWorld(), DialogueWidgetClass);
        if (DialogueWidget)
        {
            DialogueWidget->AddToViewport();
            DialogueWidget->SetIsOptional(bOptional);
            DialogueWidget->StartDialogue(DialogueToPlay);
        }
    }
    else 
    {
        // Opzionale: stampa un log o fai un piccolo feedback sonoro se vuoi
        UE_LOG(LogTemp, Log, TEXT("NPC non ha più nulla da dire."));
    }
}
