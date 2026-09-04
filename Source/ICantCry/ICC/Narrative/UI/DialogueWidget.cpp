// Fill out your copyright notice in the Description page of Project Settings.

#include "DialogueWidget.h"




#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "ICantCry/ICC/Narrative/Data/DialogueAsset.h"
#include "ICantCry/ICC/Narrative/GameplayEvent.h"
#include "ICantCry/ICC/Narrative/UI/DialogueChoiceButton.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"


void UDialogueWidget::NativeConstruct() 
{
    Super::NativeConstruct();

    // bind the continue button to advance to the next line
    if (BtnNext)   BtnNext->OnClicked.AddDynamic(this, &UDialogueWidget::DisplayNextLine);
    
    // bind accept and decline buttons but keep them hidden until needed for optional quests
    if (BtnAccept) BtnAccept->OnClicked.AddDynamic(this, &UDialogueWidget::OnAcceptClicked);
    if (BtnDecline) BtnDecline->OnClicked.AddDynamic(this, &UDialogueWidget::OnDeclineClicked);
    if (BtnAccept) BtnAccept->SetVisibility(ESlateVisibility::Collapsed);
    if (BtnDecline) BtnDecline->SetVisibility(ESlateVisibility::Collapsed);

    // bind the delivery button but keep it hidden until a turn-in quest is active
    if (BtnDeliver) BtnDeliver->OnClicked.AddDynamic(this, &UDialogueWidget::OnDeliverClicked);
    if (BtnDeliver) BtnDeliver->SetVisibility(ESlateVisibility::Collapsed);
    if (TextDeliveryProgress) TextDeliveryProgress->SetVisibility(ESlateVisibility::Collapsed);
        
    // switch to UI input mode so the player can interact with buttons using mouse or gamepad
    APlayerController* PC = GetOwningPlayer();
    if (PC) 
    { 
        PC->bShowMouseCursor = true; 
        FInputModeUIOnly Mode; 
        Mode.SetWidgetToFocus(TakeWidget()); 
        PC->SetInputMode(Mode); 
    }

    // stop player while dialogue is open
    if (AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn()))
    {
        Player->SetDialogueMovementLock(true);
    }
}

void UDialogueWidget::StartDialogue(UDialogueAsset* NewDialogue) 
{
    if (!NewDialogue) return;

    CurrentDialogue = NewDialogue;
    CurrentLineIndex = 0; // always start from the beginning of the dialogue
    
    // clear any leftover delivery ui from previous conversations
    if (BtnDeliver) BtnDeliver->SetVisibility(ESlateVisibility::Collapsed);
    if (TextDeliveryProgress) TextDeliveryProgress->SetVisibility(ESlateVisibility::Collapsed);

    // apply the font and color settings defined in the dialogue asset
    ApplyDialogueStyle(); 
    
    // show the first line, displaynextline will also handle executing any events attached to line 0
    DisplayNextLine();
}

void UDialogueWidget::DisplayNextLine() 
{
   
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        // PROTEZIONE DOPPIO CLIC FANTASMA:
        // Completiamo la linea subito solo se ha iniziato effettivamente a digitare (Index > 0).
        // Se è a 0, è una chiamata duplicata nello stesso frame dell'avvio della nuova linea e la ignoriamo.
        if (CurrentCharacterIndex > 0)
        {
            FinishLineInstantly();
        }
        return;
    }

    
    if (!CurrentDialogue)
    {
        EndDialogue();
        return;
    }

    
    if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex))
    {
        const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

        
        GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
        CurrentCharacterIndex = 0; 
        if (TextDialogueContent)
            TextDialogueContent->SetText(FText::GetEmpty());

        // Esegue gli eventi della linea
        AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn());
        for (UGameplayEvent* Event : CurrentLine.Events)
        {
            if (Event) Event->ExecuteEvent(Player, this);
        }

        // Controlla se una consegna è attiva (nasconde il tasto Next)
        UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
        UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
        bool bIsDeliveryActive = false;
        if (QM && CurrentQuestTag.IsValid())
        {
            int32 Progress = QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag);
            if (Progress < CurrentAmountRequired)
                bIsDeliveryActive = true;
        }
        if (BtnNext)
            BtnNext->SetVisibility(bIsDeliveryActive ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

        ApplyDialogueStyle();

        // Memorizza il testo completo per la digitazione
        FullTextCurrentLine = CurrentLine.Text.ToString();

        // Imposta nome e ritratto (Player o NPC)
        if (CurrentLine.bIsPlayerLine && CurrentDialogue->PlayerProfile)
        {
            if (TextNPCName)
                TextNPCName->SetText(CurrentDialogue->PlayerProfile->PlayerName);
            if (NPCFaceImage)
            {
                UTexture2D* LoadedTexture = nullptr;
                FGameplayTag TargetTag = CurrentLine.EmotionTag;
                if (CurrentDialogue->PlayerProfile->Portraits.Contains(TargetTag))
                    LoadedTexture = CurrentDialogue->PlayerProfile->Portraits[TargetTag].LoadSynchronous();
                else if (CurrentDialogue->PlayerProfile->Portraits.Num() > 0)
                {
                    TArray<FGameplayTag> Keys;
                    CurrentDialogue->PlayerProfile->Portraits.GetKeys(Keys);
                    LoadedTexture = CurrentDialogue->PlayerProfile->Portraits[Keys[0]].LoadSynchronous();
                }
                NPCFaceImage->SetBrushFromTexture(LoadedTexture);
            }
        }
        else if (CurrentDialogue->NPCProfile)
        {
            if (TextNPCName)
                TextNPCName->SetText(CurrentDialogue->NPCProfile->NPCName);
            if (NPCFaceImage)
            {
                UTexture2D* LoadedTexture = nullptr;
                FGameplayTag TargetTag = CurrentLine.EmotionTag;
                if (CurrentDialogue->NPCProfile->Portraits.Contains(TargetTag))
                    LoadedTexture = CurrentDialogue->NPCProfile->Portraits[TargetTag].LoadSynchronous();
                else if (CurrentDialogue->NPCProfile->Portraits.Num() > 0)
                {
                    TArray<FGameplayTag> Keys;
                    CurrentDialogue->NPCProfile->Portraits.GetKeys(Keys);
                    LoadedTexture = CurrentDialogue->NPCProfile->Portraits[Keys[0]].LoadSynchronous();
                }
                NPCFaceImage->SetBrushFromTexture(LoadedTexture);
            }
        }

        // Mostra il testo (con o senza effetto)
        if (CurrentDialogue->bUseTypewriterEffect && TextDialogueContent)
        {
            float Speed = FMath::Max(0.01f, CurrentDialogue->TypewriterSpeed);
            GetWorld()->GetTimerManager().SetTimer(TypewriterTimerHandle, this, &UDialogueWidget::OnTypewriterTick, Speed, true);
        }
        else if (TextDialogueContent)
        {
            TextDialogueContent->SetText(CurrentLine.Text);
        }

        // Passa alla prossima linea
        CurrentLineIndex++;
    }
    else
    {
        // Nessun'altra linea: gestisci la fine dialogo
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
    // close the widget without triggering any ondialogueended events
    // this means the quest won't start or progress
    APlayerController* PC = GetOwningPlayer();
    if (PC) 
    { 
        PC->bShowMouseCursor = false; 
        FInputModeGameOnly Mode; 
        PC->SetInputMode(Mode); 
    }

    if (AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn()))
    {
        Player->SetDialogueMovementLock(false);
    }

    RemoveFromParent();
}

void UDialogueWidget::EndDialogue() 
{

    AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn());
    if (!Player)
    {
        if (APlayerController* OwningPC = GetOwningPlayer())
            Player = Cast<AICC_Player>(OwningPC->GetPawn());
    }
    if (!Player)
        Player = Cast<AICC_Player>(UGameplayStatics::GetPlayerPawn(this, 0));


    UDialogueAsset* FinishedDialogue = CurrentDialogue;

   
    if (Player) Player->SetDialogueMovementLock(false);

    if (APlayerController* PC = GetOwningPlayer())
    {
        PC->bShowMouseCursor = false;
        FInputModeGameOnly Mode;
        PC->SetInputMode(Mode);
    }
    RemoveFromParent();

   
    if (FinishedDialogue)
    {
        const int32 EventCount = FinishedDialogue->OnDialogueEnded.Num();
        UE_LOG(LogTemp, Log, TEXT("EndDialogue: eseguo %d evento/i OnDialogueEnded (Player=%s)"),
            EventCount, Player ? *Player->GetName() : TEXT("NULL"));

        for (UGameplayEvent* Event : FinishedDialogue->OnDialogueEnded)
        {
            if (!Event)
            {
                UE_LOG(LogTemp, Warning, TEXT("EndDialogue: entry NULLA in OnDialogueEnded."));
                continue;
            }
            //Event->ExecuteEvent(Player, FinishedDialogue);
            Event->ExecuteEvent(Player, this);
        }
    }
}


void UDialogueWidget::ApplyDialogueStyle()
{
    if (!CurrentDialogue || !TextDialogueContent) return;

    // convert the linear color to slate color and apply it to the dialogue text
    TextDialogueContent->SetColorAndOpacity(FSlateColor(CurrentDialogue->DialogueColor));

    // if the dialogue asset specifies a font, apply it
    if (CurrentDialogue->DialogueFont.HasValidFont())
    {
        TextDialogueContent->SetFont(CurrentDialogue->DialogueFont);
    }
}


void UDialogueWidget::OnTypewriterTick()
{
    // add one character to the displayed text each tick until we reach the full line
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
        // once the full line is shown, stop the timer
        GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    }
}

void UDialogueWidget::FinishLineInstantly()
{
    // stop the typewriter timer and show the full line immediately
    GetWorld()->GetTimerManager().ClearTimer(TypewriterTimerHandle);
    if (TextDialogueContent)
    {
        TextDialogueContent->SetText(FText::FromString(FullTextCurrentLine));
    }
    // don't increment currentlineindex here, the next click on displaynextline will advance
}


void UDialogueWidget::ShowBranches()
{
    // validate that we have everything needed to show choices
    if (!ChoiceContainer || !CurrentDialogue || !ChoiceButtonClass) return;

    // clear any old choice buttons and make the container visible
    ChoiceContainer->SetVisibility(ESlateVisibility::Visible);
    ChoiceContainer->ClearChildren();
    
    // hide the next button because the player must make a choice now
    if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);

    // create a button for each branch defined in the dialogue asset
    for (const FDialogueBranch& Branch : CurrentDialogue->Branches)
    {
        UDialogueChoiceButton* NewButton = CreateWidget<UDialogueChoiceButton>(this, ChoiceButtonClass);
        if (NewButton)
        {
            NewButton->Setup(Branch.ReplyText, Branch.NextDialogue, this);
            ChoiceContainer->AddChildToVerticalBox(NewButton);
        }
    }
}

void UDialogueWidget::OnBranchSelected(UDialogueAsset* NextDialogue)
{
    // clean up the choice buttons
    if (ChoiceContainer)
    {
        ChoiceContainer->ClearChildren();
        ChoiceContainer->SetVisibility(ESlateVisibility::Collapsed);
    }

    // if the branch leads to another dialogue, start it
    if (NextDialogue)
    {
        StartDialogue(NextDialogue);
    }
    else
    {
        // if no next dialogue, end the conversation
        EndDialogue();
    }
}


void UDialogueWidget::UpdateDeliveryUI()
{
    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
    
    if (GI && QM && TextDeliveryProgress)
    {
        int32 AlreadyDelivered = QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag);
        int32 InInventory = GI->GetItemCount(CurrentRequiredItemTag);
        bool bFinished = AlreadyDelivered >= CurrentAmountRequired;

        // update the progress text with current numbers
        TextDeliveryProgress->SetText(FText::FromString(FString::Printf(TEXT("%s given: %d/%d"), 
            *CurrentRequiredItemTag.GetTagName().ToString(), AlreadyDelivered, CurrentAmountRequired)));
        TextDeliveryProgress->SetVisibility(ESlateVisibility::Visible);

        // turn the text green when the delivery is complete
        TextDeliveryProgress->SetColorAndOpacity(bFinished ? FLinearColor::Green : FLinearColor::White);

        // manage delivery button visibility and enabled state
        if (BtnDeliver)
        {
            BtnDeliver->SetVisibility(bFinished ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
            BtnDeliver->SetIsEnabled(InInventory > 0 && !bFinished);
        }

        // show the next button only when delivery is complete so the player can continue
        if (BtnNext)
        {
            BtnNext->SetVisibility(bFinished ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
        }
    }
}


void UDialogueWidget::OnDeliverClicked()
{
    UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(GetGameInstance());
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;

    // try to remove one item from inventory and update progress if successful
    if (GI && QM && GI->RemoveFromInventory(CurrentRequiredItemTag, 1))
    {
        QM->UpdateObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag, 1);

        // Once the delivery is complete UpdateDeliveryUI already shows BtnNext, so the player
        // advances with a click. No timer here: a delayed DisplayNextLine would race with that
        // click and skip a line.
        UpdateDeliveryUI();
    }
}