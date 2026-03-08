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

void UDialogueWidget::NativeConstruct() 
{
    Super::NativeConstruct();

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
   // 1. GESTIONE INPUT DURANTE LA SCRITTURA
    // Se il testo sta ancora scorrendo, lo mostriamo tutto subito e interrompiamo il timer
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        FinishLineInstantly();
        return;
    }

    // Validità del dialogo e del profilo NPC
    if (!CurrentDialogue || !CurrentDialogue->NPCProfile) 
    {
        EndDialogue();
        return;
    }

    // 2. LOGICA VISUALIZZAZIONE RIGA
    if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    {
        const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];
        FullTextCurrentLine = CurrentLine.Text.ToString();
        CurrentCharacterIndex = 0;

        // FIX: Assicuriamoci che il tasto Next sia visibile per scorrere le righe
        if (BtnNext) 
        {
            BtnNext->SetVisibility(ESlateVisibility::Visible);
        }

        // AGGIORNAMENTO UI: Nome e Ritratto NPC
        if (CurrentDialogue->NPCProfile)
        {
            // Imposta il Nome
            if (TextNPCName)
            {
                TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
            }

            // Imposta il Ritratto (Portrait) in base all'emozione
            if (NPCFaceImage)
            {
                FGameplayTag TargetTag = CurrentLine.EmotionTag;
                
                // Cerchiamo l'emozione nella mappa Portraits del profilo
                if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
                {
                    UTexture2D* LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
                    NPCFaceImage->SetBrushFromTexture(LoadedTexture);
                }
                else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
                {
                    // Fallback: se il tag non esiste, prendi il primo ritratto disponibile
                    TArray<FGameplayTag> OutKeys;
                    CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
                    UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
                    NPCFaceImage->SetBrushFromTexture(DefaultTexture);
                }
            }
        }

        // GESTIONE TESTO (Macchina da scrivere vs Istantaneo)
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
        // 3. LOGICA DI FINE DIALOGO / USCITA
        
        // Se l'asset corrente vieta i tasti missione (es. dialogo di rifiuto), chiudi tutto
        if (CurrentDialogue->bNeverShowQuestButtons)
        {
            EndDialogue();
            return;
        }
        
        // PRIORITÀ 1: BRANCHES (Se ci sono bivi, mostriamo le scelte)
        if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
        {
            if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
            ShowBranches();
        }
        // PRIORITÀ 2: QUEST OPZIONALE (Mostra Accept/Decline se non ci sono bivi)
        else if (bIsOptionalQuest) 
        {
            if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
            if (BtnAccept) 
            { 
                BtnAccept->SetVisibility(ESlateVisibility::Visible); 
                BtnAccept->SetFocus(); // Focus per supporto Gamepad/Tastiera
            }
            if (BtnDecline) 
            {
                BtnDecline->SetVisibility(ESlateVisibility::Visible);
            }
        }
        // PRIORITÀ 3: FINE DIALOGO STANDARD
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