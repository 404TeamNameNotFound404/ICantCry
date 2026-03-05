// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueWidget.h"



#include "ICantCry/ICC/Narrative/UI/DialogueWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/Data/NPCProfile.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"

void UDialogueWidget::NativeConstruct() 
{
    Super::NativeConstruct();

    

    // APlayerController* PC = GetOwningPlayer();
    // if (PC)
    // {
    //     // 1. Permette al mouse di interagire con il Widget
    //     FInputModeUIOnly InputMode;
    //     InputMode.SetWidgetToFocus(TakeWidget());
    //     InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
    //     PC->SetInputMode(InputMode);

    //     // 2. Rende il cursore visibile
    //     PC->bShowMouseCursor = true;
    // }

    // if (BtnNext) 
    // {
    //     BtnNext->OnClicked.AddDynamic(this, &UDialogueWidget::DisplayNextLine);
    // }


    if (BtnNext)   BtnNext->OnClicked.AddDynamic(this, &UDialogueWidget::DisplayNextLine);
    if (BtnAccept) BtnAccept->OnClicked.AddDynamic(this, &UDialogueWidget::OnAcceptClicked);
    if (BtnDecline) BtnDecline->OnClicked.AddDynamic(this, &UDialogueWidget::OnDeclineClicked);

    // Default: bottoni scelta nascosti
    if (BtnAccept) BtnAccept->SetVisibility(ESlateVisibility::Collapsed);
    if (BtnDecline) BtnDecline->SetVisibility(ESlateVisibility::Collapsed);

    // Setup Input
    APlayerController* PC = GetOwningPlayer();
    if (PC) { PC->bShowMouseCursor = true; FInputModeUIOnly Mode; Mode.SetWidgetToFocus(TakeWidget()); PC->SetInputMode(Mode); }
}

void UDialogueWidget::StartDialogue(UDialogueAsset* NewDialogue) 
{
    if (!NewDialogue) return;

   CurrentDialogue = NewDialogue;
    CurrentLineIndex = 0;

    ApplyDialogueStyle();
    
    if (TextNPCName && CurrentDialogue->NPCProfile) 
        TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
    
    DisplayNextLine();
}

void UDialogueWidget::DisplayNextLine() 
{

//    // Controllo di sicurezza iniziale
//     if (!CurrentDialogue || !CurrentDialogue->NPCProfile) 
//     {
//         EndDialogue();
//         return;
//     }

//     // 1. Controlliamo se ci sono ancora righe di testo da visualizzare
//     if (CurrentDialogue && CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
//     {
//         const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

//         // Aggiorna il Testo principale del Widget
//         if (TextDialogueContent)
//         {
//             TextDialogueContent->SetText(CurrentLine.Text);
//         }

//         // LOGICA PORTRAIT: Cambia l'immagine dell'NPC in base all'emozione della riga
//         if (NPCFaceImage && CurrentDialogue->NPCProfile)
//         {
//             FGameplayTag Emotion = CurrentLine.EmotionTag;
            
//             // Cerchiamo se nel profilo dell'NPC esiste una texture per questa emozione
//             if (CurrentDialogue->NPCProfile->Portraits.Contains(Emotion))
//             {
//                 // Carichiamo la texture (essendo un SoftObjectPtr usiamo LoadSynchronous)
//                 UTexture2D* LoadedTex = CurrentDialogue->NPCProfile->Portraits[Emotion].LoadSynchronous();
//                 if (LoadedTex)
//                 {
//                     NPCFaceImage->SetBrushFromTexture(LoadedTex);
//                 }
//             }
//         }

//         // Incrementiamo l'indice per la prossima riga
//         CurrentLineIndex++;
//     } 
//     else 
//     {
//         // 2. FINE DEL DIALOGO: Gestione della scelta o della chiusura automatica
//         if (bIsOptionalQuest) 
//         {
//             // Se la missione è opzionale, nascondiamo il tasto "Avanti"
//             if (BtnNext) 
//             {
//                 BtnNext->SetVisibility(ESlateVisibility::Collapsed);
//             }

//             // Mostriamo i tasti Accetta e Rifiuta
//             if (BtnAccept) 
//             {
//                 BtnAccept->SetVisibility(ESlateVisibility::Visible);
//                 BtnAccept->SetFocus(); // Imposta il focus per navigazione con gamepad/tastiera
//             }

//             if (BtnDecline) 
//             {
//                 BtnDecline->SetVisibility(ESlateVisibility::Visible);
//             }
//         }
//         else 
//         {
//             // Se la missione è obbligatoria (o è un dialogo normale), chiudiamo ed eseguiamo gli eventi
//             EndDialogue();
//         }
//     }



    // 1. Se il timer è attivo, significa che il testo sta ancora scrivendo.
    // In questo caso, completiamo la riga istantaneamente e usciamo.
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        FinishLineInstantly();
        return;
    }

    // Controllo di sicurezza iniziale
    if (!CurrentDialogue || !CurrentDialogue->NPCProfile) 
    {
        EndDialogue();
        return;
    }

    // 2. Controlliamo se ci sono ancora righe di testo da visualizzare
    if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    {
        const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

        // Salviamo il testo completo per il timer o per il completamento rapido
        FullTextCurrentLine = CurrentLine.Text.ToString();
        CurrentCharacterIndex = 0;

        // LOGICA PORTRAIT (rimane invariata)
        if (NPCFaceImage && CurrentDialogue->NPCProfile)
        {
            FGameplayTag Emotion = CurrentLine.EmotionTag;
            if (CurrentDialogue->NPCProfile->Portraits.Contains(Emotion))
            {
                UTexture2D* LoadedTex = CurrentDialogue->NPCProfile->Portraits[Emotion].LoadSynchronous();
                if (LoadedTex)
                {
                    NPCFaceImage->SetBrushFromTexture(LoadedTex);
                }
            }
        }

        // --- NUOVA LOGICA MACCHINA DA SCRIVERE ---
        if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
        {
            // Puliamo il testo e avviamo il timer
            TextDialogueContent->SetText(FText::GetEmpty());
            
            float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
            GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
        }
        else
        {
            // Se l'effetto è disattivato, mostriamo tutto subito
            if (TextDialogueContent)
            {
                TextDialogueContent->SetText(CurrentLine.Text);
            }
        }

        // Incrementiamo l'indice per la prossima riga
        CurrentLineIndex++;
    } 
    else 
    {
        // 3. FINE DEL DIALOGO (rimane invariata)
        if (bIsOptionalQuest) 
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