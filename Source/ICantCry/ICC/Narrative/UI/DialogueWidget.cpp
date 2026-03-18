// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueWidget.h"



#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/UI/DialogueChoiceButton.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"


void UDialogueWidget::NativeConstruct() 
{
    Super::NativeConstruct();

    // Continue
    if (BtnNext)   BtnNext->OnClicked.AddDynamic(this, &UDialogueWidget::DisplayNextLine);
    

    // ACCept or Decline
    if (BtnAccept) BtnAccept->OnClicked.AddDynamic(this, &UDialogueWidget::OnAcceptClicked);
    if (BtnDecline) BtnDecline->OnClicked.AddDynamic(this, &UDialogueWidget::OnDeclineClicked);
    if (BtnAccept) BtnAccept->SetVisibility(ESlateVisibility::Collapsed);
    if (BtnDecline) BtnDecline->SetVisibility(ESlateVisibility::Collapsed);

    // Delivery 
    if (BtnDeliver) BtnDeliver->OnClicked.AddDynamic(this, &UDialogueWidget::OnDeliverClicked);
    if (BtnDeliver) BtnDeliver->SetVisibility(ESlateVisibility::Collapsed);
    if (TextDeliveryProgress) TextDeliveryProgress->SetVisibility(ESlateVisibility::Collapsed);
        

    // Setup Input
    APlayerController* PC = GetOwningPlayer();
    if (PC) { PC->bShowMouseCursor = true; FInputModeUIOnly Mode; Mode.SetWidgetToFocus(TakeWidget()); PC->SetInputMode(Mode); }
}

void UDialogueWidget::StartDialogue(UDialogueAsset* NewDialogue) 
{
    //     if (!NewDialogue) return;

    //    CurrentDialogue = NewDialogue;
    //     CurrentLineIndex = 0;

    //     ApplyDialogueStyle();
    
    //     if (TextNPCName && CurrentDialogue->NPCProfile) 
    //         TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
    
    //     DisplayNextLine();


    //---------- QUESTO é GIUSTO

    // CurrentDialogue = NewDialogue;
    // CurrentLineIndex = 0;

    // if (CurrentDialogue && CurrentDialogue->Lines.Num() > 0)
    // {
    //     ApplyDialogueStyle(); // Mostra testo/immagine riga 0

    //     // Esegui subito gli eventi della riga 0 (es. PrepareDelivery)
    //     for (UGameplayEvent* Event : CurrentDialogue->Lines[CurrentLineIndex].Events)
    //     {
    //         if (Event) Event->ExecuteEvent(nullptr, this);
    //     }
    // }

    //---------- 


    if (!NewDialogue) return;

    CurrentDialogue = NewDialogue;
    CurrentLineIndex = 0; // Partiamo dall'inizio

    // Reset della UI di consegna se era rimasta sporca da dialoghi precedenti
    if (BtnDeliver) BtnDeliver->SetVisibility(ESlateVisibility::Collapsed);
    if (TextDeliveryProgress) TextDeliveryProgress->SetVisibility(ESlateVisibility::Collapsed);

    // Applichiamo lo stile globale del dialogo (Font/Colore)
    ApplyDialogueStyle(); 
    
    // Mostriamo la prima riga (Index 0). 
    // DisplayNextLine si occuperà di eseguire gli eventi della riga 0.
    DisplayNextLine();
}

void UDialogueWidget::DisplayNextLine() 
{
//    // 1. GESTIONE INPUT DURANTE LA SCRITTURA
//     // Se il testo sta ancora scorrendo, lo mostriamo tutto subito e interrompiamo il timer
//     if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
//     {
//         FinishLineInstantly();
//         return;
//     }

//     // Validità del dialogo e del profilo NPC
//     if (!CurrentDialogue || !CurrentDialogue->NPCProfile) 
//     {
//         EndDialogue();
//         return;
//     }

//     // 2. LOGICA VISUALIZZAZIONE RIGA
//     if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
//     {
//         const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];
//         FullTextCurrentLine = CurrentLine.Text.ToString();
//         CurrentCharacterIndex = 0;

//         // FIX: Assicuriamoci che il tasto Next sia visibile per scorrere le righe
//         if (BtnNext) 
//         {
//             BtnNext->SetVisibility(ESlateVisibility::Visible);
//         }

//         // AGGIORNAMENTO UI: Nome e Ritratto NPC
//         if (CurrentDialogue->NPCProfile)
//         {
//             // Imposta il Nome
//             if (TextNPCName)
//             {
//                 TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
//             }

//             // Imposta il Ritratto (Portrait) in base all'emozione
//             if (NPCFaceImage)
//             {
//                 FGameplayTag TargetTag = CurrentLine.EmotionTag;
                
//                 // Cerchiamo l'emozione nella mappa Portraits del profilo
//                 if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
//                 {
//                     UTexture2D* LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
//                     NPCFaceImage->SetBrushFromTexture(LoadedTexture);
//                 }
//                 else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
//                 {
//                     // Fallback: se il tag non esiste, prendi il primo ritratto disponibile
//                     TArray<FGameplayTag> OutKeys;
//                     CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
//                     UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
//                     NPCFaceImage->SetBrushFromTexture(DefaultTexture);
//                 }
//             }
//         }

//         // GESTIONE TESTO (Macchina da scrivere vs Istantaneo)
//         if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
//         {
//             TextDialogueContent->SetText(FText::GetEmpty());
//             float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
//             GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
//         }
//         else if (TextDialogueContent)
//         {
//             TextDialogueContent->SetText(CurrentLine.Text);
//         }

//         CurrentLineIndex++;
//     } 
//     else 
//     {
//         // 3. LOGICA DI FINE DIALOGO / USCITA
        
//         // Se l'asset corrente vieta i tasti missione (es. dialogo di rifiuto), chiudi tutto
//         if (CurrentDialogue->bNeverShowQuestButtons)
//         {
//             EndDialogue();
//             return;
//         }
        
//         // PRIORITÀ 1: BRANCHES (Se ci sono bivi, mostriamo le scelte)
//         if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
//         {
//             if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
//             ShowBranches();
//         }
//         // PRIORITÀ 2: QUEST OPZIONALE (Mostra Accept/Decline se non ci sono bivi)
//         else if (bIsOptionalQuest) 
//         {
//             if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
//             if (BtnAccept) 
//             { 
//                 BtnAccept->SetVisibility(ESlateVisibility::Visible); 
//                 BtnAccept->SetFocus(); // Focus per supporto Gamepad/Tastiera
//             }
//             if (BtnDecline) 
//             {
//                 BtnDecline->SetVisibility(ESlateVisibility::Visible);
//             }
//         }
//         // PRIORITÀ 3: FINE DIALOGO STANDARD
//         else 
//         {
//             EndDialogue();
//         }
//     }


    //__________GIUSTO

    // // 1. GESTIONE INPUT DURANTE LA SCRITTURA
    // if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    // {
    //     FinishLineInstantly();
    //     return;
    // }

    // // Validità del dialogo
    // if (!CurrentDialogue) 
    // {
    //     EndDialogue();
    //     return;
    // }

    // // 2. LOGICA VISUALIZZAZIONE RIGA
    // if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    // {
    //     const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];
    //     FullTextCurrentLine = CurrentLine.Text.ToString();
    //     CurrentCharacterIndex = 0;

    //     // --- NOVITÀ: ESECUZIONE EVENTI DELLA RIGA CORRENTE ---
    //     // Prima di mostrare il testo, eseguiamo gli eventi (es. PrepareDelivery)
    //     for (UGameplayEvent* Event : CurrentLine.Events)
    //     {
    //         if (Event)
    //         {
    //             // Passiamo 'this' come context affinché l'evento possa modificare questo widget
    //             Event->ExecuteEvent(nullptr, this);
    //         }
    //     }

    //     ApplyDialogueStyle();

    //     // RESET UI: Se la riga precedente aveva nascosto il Next (per una consegna), 
    //     // lo ripristiniamo a meno che un evento della riga attuale non lo nasconda di nuovo.
    //     if (BtnNext) 
    //     {
    //         BtnNext->SetVisibility(ESlateVisibility::Visible);
    //     }

    //     // AGGIORNAMENTO UI: Nome e Ritratto NPC
    //     if (CurrentDialogue->NPCProfile)
    //     {
    //         if (TextNPCName)
    //         {
    //             TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
    //         }

    //         if (NPCFaceImage)
    //         {
    //             FGameplayTag TargetTag = CurrentLine.EmotionTag;
                
    //             if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
    //             {
    //                 UTexture2D* LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
    //                 NPCFaceImage->SetBrushFromTexture(LoadedTexture);
    //             }
    //             else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
    //             {
    //                 TArray<FGameplayTag> OutKeys;
    //                 CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
    //                 UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
    //                 NPCFaceImage->SetBrushFromTexture(DefaultTexture);
    //             }
    //         }
    //     }

    //     // GESTIONE TESTO
    //     if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
    //     {
    //         TextDialogueContent->SetText(FText::GetEmpty());
    //         float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
    //         GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
    //     }
    //     else if (TextDialogueContent)
    //     {
    //         TextDialogueContent->SetText(CurrentLine.Text);
    //     }

    //     CurrentLineIndex++;
    // } 
    // else 
    // {
    //     // 3. LOGICA DI FINE DIALOGO / USCITA
    //     if (CurrentDialogue->bNeverShowQuestButtons)
    //     {
    //         EndDialogue();
    //         return;
    //     }
        
    //     if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
    //     {
    //         if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
    //         ShowBranches();
    //     }
    //     else if (bIsOptionalQuest) 
    //     {
    //         if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
    //         if (BtnAccept) 
    //         { 
    //             BtnAccept->SetVisibility(ESlateVisibility::Visible); 
    //             BtnAccept->SetFocus();
    //         }
    //         if (BtnDecline) 
    //         {
    //             BtnDecline->SetVisibility(ESlateVisibility::Visible);
    //         }
    //     }
    //     else 
    //     {
    //         EndDialogue();
    //     }
    // }


    //------------------------

    // // 1. GESTIONE INPUT DURANTE LA SCRITTURA
    // // Se il testo sta ancora scorrendo, lo mostriamo subito e fermiamo il timer
    // if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    // {
    //     FinishLineInstantly();
    //     return;
    // }

    // // Validità del dialogo
    // if (!CurrentDialogue) 
    // {
    //     EndDialogue();
    //     return;
    // }

    // // 2. LOGICA VISUALIZZAZIONE RIGA
    // if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    // {
    //     const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

    //     // --- ESECUZIONE EVENTI DELLA RIGA CORRENTE ---
    //     // Eseguiamo gli eventi (es. PrepareDelivery) prima di mostrare il testo
    //     for (UGameplayEvent* Event : CurrentLine.Events)
    //     {
    //         if (Event)
    //         {
    //             // Passiamo 'this' come context per permettere all'evento di modificare il widget
    //             Event->ExecuteEvent(nullptr, this);
    //         }
    //     }

    //     // Applichiamo font e colore definiti nell'asset
    //     ApplyDialogueStyle();

    //     // RESET UI: Ripristiniamo il tasto Next. 
    //     // Se un evento appena eseguito (come PrepareDelivery) deve nasconderlo, 
    //     // lo farà subito dopo chiamando UpdateDeliveryUI.
    //     if (BtnNext) 
    //     {
    //         BtnNext->SetVisibility(ESlateVisibility::Visible);
    //     }

    //     // Preparazione dati per il testo
    //     FullTextCurrentLine = CurrentLine.Text.ToString();
    //     CurrentCharacterIndex = 0;

    //     // AGGIORNAMENTO UI: Nome e Ritratto NPC
    //     if (CurrentDialogue->NPCProfile)
    //     {
    //         if (TextNPCName)
    //         {
    //             TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
    //         }

    //         if (NPCFaceImage)
    //         {
    //             FGameplayTag TargetTag = CurrentLine.EmotionTag;
                
    //             if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
    //             {
    //                 UTexture2D* LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
    //                 NPCFaceImage->SetBrushFromTexture(LoadedTexture);
    //             }
    //             else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
    //             {
    //                 // Fallback al primo ritratto disponibile
    //                 TArray<FGameplayTag> OutKeys;
    //                 CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
    //                 UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
    //                 NPCFaceImage->SetBrushFromTexture(DefaultTexture);
    //             }
    //         }
    //     }

    //     // GESTIONE TESTO (Macchina da scrivere o istantaneo)
    //     if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
    //     {
    //         TextDialogueContent->SetText(FText::GetEmpty());
    //         float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
    //         GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
    //     }
    //     else if (TextDialogueContent)
    //     {
    //         TextDialogueContent->SetText(CurrentLine.Text);
    //     }

    //     // Incrementiamo l'indice per la prossima chiamata
    //     CurrentLineIndex++;
    // } 
    // else 
    // {
    //     // 3. LOGICA DI FINE DIALOGO / USCITA
    //     if (CurrentDialogue->bNeverShowQuestButtons)
    //     {
    //         EndDialogue();
    //         return;
    //     }
        
    //     // Priorità 1: Branches (Bivi decisionali)
    //     if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
    //     {
    //         if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
    //         ShowBranches();
    //     }
    //     // Priorità 2: Quest opzionale (Accetta/Rifiuta)
    //     else if (bIsOptionalQuest) 
    //     {
    //         if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
    //         if (BtnAccept) 
    //         { 
    //             BtnAccept->SetVisibility(ESlateVisibility::Visible); 
    //             BtnAccept->SetFocus();
    //         }
    //         if (BtnDecline) 
    //         {
    //             BtnDecline->SetVisibility(ESlateVisibility::Visible);
    //         }
    //     }
    //     // Priorità 3: Fine standard
    //     else 
    //     {
    //         EndDialogue();
    //     }
    // }




    //&&&&&&&&&&&&&&&&&&&

    // 1. GESTIONE INPUT DURANTE LA SCRITTURA
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        FinishLineInstantly();
        return;
    }

    if (!CurrentDialogue) 
    {
        EndDialogue();
        return;
    }

    // 2. LOGICA VISUALIZZAZIONE RIGA
    if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    {
        const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

        // --- 1. ESECUZIONE EVENTI ---
        // Eseguiamo gli eventi (es. PrepareDelivery) per aggiornare i tag di missione nel widget
        for (UGameplayEvent* Event : CurrentLine.Events)
        {
            if (Event)
            {
                Event->ExecuteEvent(nullptr, this);
            }
        }

        // --- 2. CONTROLLO STATO CONSEGNA PER VISIBILITÀ BTNNEXT ---
        // Verifichiamo se questa riga ha attivato una consegna non ancora completata
        UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
        UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
        
        bool bIsDeliveryActive = false;
        if (QM && CurrentQuestTag.IsValid())
        {
            int32 Progress = QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag);
            if (Progress < CurrentAmountRequired)
            {
                bIsDeliveryActive = true;
            }
        }

        if (BtnNext) 
        {
            // Se c'è una consegna in corso, il tasto Next deve restare nascosto (Collapsed)
            // altrimenti lo rendiamo visibile per procedere.
            BtnNext->SetVisibility(bIsDeliveryActive ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }

        // --- 3. AGGIORNAMENTO UI E TESTO ---
        ApplyDialogueStyle();

        FullTextCurrentLine = CurrentLine.Text.ToString();
        CurrentCharacterIndex = 0;

        if (CurrentDialogue->NPCProfile)
        {
            if (TextNPCName) TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);

            if (NPCFaceImage)
            {
                FGameplayTag TargetTag = CurrentLine.EmotionTag;
                if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
                {
                    UTexture2D* LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
                    NPCFaceImage->SetBrushFromTexture(LoadedTexture);
                }
                else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
                {
                    TArray<FGameplayTag> OutKeys;
                    CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
                    UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
                    NPCFaceImage->SetBrushFromTexture(DefaultTexture);
                }
            }
        }

        if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
        {
            TextDialogueContent->SetText(FText::GetEmpty());
            float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
            GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
        }
        else if (TextDialogueContent)
        {
            TextDialogueContent->SetText(CurrentLine.Text);
        }

        CurrentLineIndex++;
    } 
    else 
    {
        // 3. LOGICA DI FINE DIALOGO
        if (CurrentDialogue->bNeverShowQuestButtons)
        {
            EndDialogue();
            return;
        }
        
        if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
        {
            if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
            ShowBranches();
        }
        else if (bIsOptionalQuest) 
        {
            if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
            if (BtnAccept) 
            { 
                BtnAccept->SetVisibility(ESlateVisibility::Visible); 
                BtnAccept->SetFocus();
            }
            if (BtnDecline) BtnDecline->SetVisibility(ESlateVisibility::Visible);
        }
        else 
        {
            EndDialogue();
        }
    }
    

}

void UDialogueWidget::OnAcceptClicked() 
{
    EndDialogue(); 
}

void UDialogueWidget::OnDeclineClicked() 
{
    // Chiude senza attivare gli eventi (la quest non parte)
    APlayerController* PC = GetOwningPlayer();
    if (PC) { PC->bShowMouseCursor = false; FInputModeGameOnly Mode; PC->SetInputMode(Mode); }
    RemoveFromParent();
}

void UDialogueWidget::EndDialogue() 
{

    // Esegue gli eventi (come StartQuest) solo se il dialogo finisce correttamente
    if (CurrentDialogue)
    {
        if (AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn()))
        {
            for (UGameplayEvent* Event : CurrentDialogue->OnDialogueEnded)
            {
                if (Event) Event->ExecuteEvent(Player, CurrentDialogue);
            }
        }
    }
    
    // Ripristino input e chiusura
    APlayerController* PC = GetOwningPlayer();
    if (PC) { PC->bShowMouseCursor = false; FInputModeGameOnly Mode; PC->SetInputMode(Mode); }
    RemoveFromParent();
}


void UDialogueWidget::ApplyDialogueStyle()
{
  if (!CurrentDialogue || !TextDialogueContent) return;

    // 1. Applica il Colore (da FLinearColor a FSlateColor)
    TextDialogueContent->SetColorAndOpacity(FSlateColor(CurrentDialogue->DialogueColor));

    // 2. Applica il Font (se impostato nel Data Asset)
    if (CurrentDialogue->DialogueFont.HasValidFont())
    {
        TextDialogueContent->SetFont(CurrentDialogue->DialogueFont);
    }
}


void UDialogueWidget::OnTypewriterTick()
{
    if (CurrentCharacterIndex < FullTextCurrentLine.Len())
    {
        CurrentCharacterIndex++;
        FString DisplayString = FullTextCurrentLine.Left(CurrentCharacterIndex);
        if (TextDialogueContent)
        {
            TextDialogueContent->SetText(FText::FromString(DisplayString));
        }
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    }
}

void UDialogueWidget::FinishLineInstantly()
{
    GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    if (TextDialogueContent)
    {
        TextDialogueContent->SetText(FText::FromString(FullTextCurrentLine));
    }
    // Non incrementiamo CurrentLineIndex qui, lo farà il prossimo click su DisplayNextLine
}


void UDialogueWidget::ShowBranches()
{
    // 1. Controlli di sicurezza
    if (!ChoiceContainer || !CurrentDialogue || !ChoiceButtonClass) return;

    // 2. Prepariamo la UI
    ChoiceContainer->SetVisibility(ESlateVisibility::Visible);
    ChoiceContainer->ClearChildren();
    
    // Disabilitiamo il tasto "Next" perché il giocatore deve scegliere tra i bivi
    if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);

    // 3. Creiamo un bottone per ogni ramo (Branch) definito nel Data Asset
    for (const FDialogueBranch& Branch : CurrentDialogue->Branches)
    {
        // Spawnamo il widget del bottone usando la classe configurata nel .h
        UDialogueChoiceButton* NewButton = CreateWidget<UDialogueChoiceButton>(this, ChoiceButtonClass);
        if (NewButton)
        {
            // Inizializziamo il bottone con il testo della risposta e il dialogo di destinazione
            NewButton->Setup(Branch.ReplyText, Branch.NextDialogue, this);
            
            // Lo aggiungiamo alla VerticalBox
            ChoiceContainer->AddChildToVerticalBox(NewButton);
        }
    }
}

void UDialogueWidget::OnBranchSelected(UDialogueAsset* NextDialogue)
{
   // Puliamo la UI delle scelte
    if (ChoiceContainer)
    {
        ChoiceContainer->ClearChildren();
        ChoiceContainer->SetVisibility(ESlateVisibility::Collapsed);
    }

    // Se la scelta porta a un nuovo dialogo, lo facciamo partire
    if (NextDialogue)
    {
        StartDialogue(NextDialogue);
    }
    else
    {
        // Se la scelta non ha un dialogo successivo, chiudiamo la conversazione
        EndDialogue();
    }
}


void UDialogueWidget::UpdateDeliveryUI()
{
//    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
//     UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
    
//     if (GI && QM && TextDeliveryProgress)
//     {
//         int32 AlreadyDelivered = QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag);
//         int32 InInventory = GI->GetItemCount(CurrentRequiredItemTag);

//         FString ItemName = CurrentRequiredItemTag.GetTagName().ToString(); 
        
//         TextDeliveryProgress->SetText(FText::FromString(FString::Printf(TEXT("%s dati: %d/%d"), 
//             *ItemName, AlreadyDelivered, CurrentAmountRequired)));

//         // Abilitiamo il tasto consegna solo se abbiamo oggetti
//         BtnDeliver->SetIsEnabled(InInventory > 0);
        
//         // Mostriamo i widget della consegna
//         BtnDeliver->SetVisibility(ESlateVisibility::Visible);
//         TextDeliveryProgress->SetVisibility(ESlateVisibility::Visible);

//         // --- AGGIUNTA CORRETTA ---
//         // Nascondiamo il tasto "Next" standard per impedire di saltare il dialogo
//         // finché la consegna non è completata.
//         if (BtnNext) 
//         {
//             BtnNext->SetVisibility(ESlateVisibility::Collapsed);
//         }
//     }

    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
    
    if (GI && QM && TextDeliveryProgress)
    {
        int32 AlreadyDelivered = QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag);
        int32 InInventory = GI->GetItemCount(CurrentRequiredItemTag);
        bool bFinished = AlreadyDelivered >= CurrentAmountRequired;

        TextDeliveryProgress->SetText(FText::FromString(FString::Printf(TEXT("%s dati: %d/%d"), 
            *CurrentRequiredItemTag.GetTagName().ToString(), AlreadyDelivered, CurrentAmountRequired)));
        TextDeliveryProgress->SetVisibility(ESlateVisibility::Visible);

        // Colore Verde se finito
        TextDeliveryProgress->SetColorAndOpacity(bFinished ? FLinearColor::Green : FLinearColor::White);

        if (BtnDeliver)
        {
            BtnDeliver->SetVisibility(bFinished ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
            // Forza l'abilitazione se hai oggetti e non hai finito
            BtnDeliver->SetIsEnabled(InInventory > 0 && !bFinished);
        }

        if (BtnNext)
        {
            // Il tasto Next riappare solo quando hai finito la consegna
            BtnNext->SetVisibility(bFinished ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }
    }
}


void UDialogueWidget::OnDeliverClicked()
{
    // UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
    // UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;

    // if (GI && QM && GI->RemoveFromInventory(CurrentRequiredItemTag, 1))
    // {
    //     QM->UpdateObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag, 1);
    //     UpdateDeliveryUI();

    //     if (QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag) >= CurrentAmountRequired)
    //     {
    //         BtnDeliver->SetVisibility(ESlateVisibility::Collapsed);
    //         TextDeliveryProgress->SetText(FText::FromString("Consegna completata!"));
            
    //         // Proseguiamo il dialogo dopo 1.5 secondi
    //         FTimerHandle TimerHandle;
    //         GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UDialogueWidget::DisplayNextLine, 1.5f, false);
    //     }
    // }

    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;

    if (GI && QM && GI->RemoveFromInventory(CurrentRequiredItemTag, 1))
    {
        QM->UpdateObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag, 1);
        UpdateDeliveryUI();

        // Se abbiamo finito (es. 3/3)
        if (QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag) >= CurrentAmountRequired)
        {
            // Aspettiamo 1 secondo (per far vedere il colore verde) e passiamo alla riga successiva
            FTimerHandle NextLineTimer;
            GetWorld()->GetTimerManager().SetTimer(NextLineTimer, this, &UDialogueWidget::DisplayNextLine, 1.0f, false);
        }
    }
}