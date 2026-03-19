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
    // if the typewriter effect is still running, instantly show the full line and stop the timer
    // this gives the player a way to skip the animation if they click again
    if (GetWorld()->GetTimerManager().IsTimerActive(TypewriterTimerHandle))
    {
        FinishLineInstantly();
        return;
    }

    // if the dialogue was somehow invalid, close the widget
    if (!CurrentDialogue) 
    {
        EndDialogue();
        return;
    }

    // check if we still have lines to display
    if (CurrentDialogue->Lines.IsValidIndex(CurrentLineIndex)) 
    {
        const FDialogueLine& CurrentLine = CurrentDialogue->Lines[CurrentLineIndex];

        // execute any gameplay events attached to this line before showing the text
        // events like preparedelivery will set up the delivery ui state by updating the quest tags in this widget
        for (UGameplayEvent* Event : CurrentLine.Events)
        {
            if (Event)
            {
                Event->ExecuteEvent(nullptr, this);
            }
        }

        // check if this line activated a delivery that isn't complete yet
        // if a delivery is in progress, the next button should stay hidden
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

        // update next button visibility based on delivery state
        if (BtnNext) 
        {
            BtnNext->SetVisibility(bIsDeliveryActive ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
        }

        // apply the dialogue style again in case it changed
        ApplyDialogueStyle();

        // store the full text for typewriter effect and reset character index
        FullTextCurrentLine = CurrentLine.Text.ToString();
        CurrentCharacterIndex = 0;

        // update npc name and portrait based on the current line's emotion tag
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
                    // if the specific emotion tag isn't found, fall back to the first available portrait
                    TArray<FGameplayTag> OutKeys;
                    CurrentDialogue->NPCProfile->Portraits.GetKeys(OutKeys);
                    UTexture2D* DefaultTexture = CurrentDialogue->NPCProfile->Portraits[OutKeys[0]].LoadSynchronous();
                    NPCFaceImage->SetBrushFromTexture(DefaultTexture);
                }
            }
        }

        // display the text either with typewriter effect or instantly
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

        // move to the next line for the next call
        CurrentLineIndex++;
    } 
    else 
    {
        // no more lines, handle end of dialogue based on quest state and dialogue settings
        
        // if this dialogue is marked to never show quest buttons, just close it
        if (CurrentDialogue->bNeverShowQuestButtons)
        {
            EndDialogue();
            return;
        }
        
        // if the dialogue has branches, show the choice buttons
        if (CurrentDialogue->bUseBranches && CurrentDialogue->Branches.Num() > 0)
        {
            if (BtnNext) BtnNext->SetVisibility(ESlateVisibility::Collapsed);
            ShowBranches();
        }
        // otherwise if this is an optional quest, show accept and decline buttons
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
        // if none of the above, just end the dialogue normally
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
    RemoveFromParent();
}

void UDialogueWidget::EndDialogue() 
{
    // trigger any events that should happen when the dialogue finishes normally
    // this is where quests are started or other narrative events happen
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
    
    // restore game input mode and close the widget
    APlayerController* PC = GetOwningPlayer();
    if (PC) 
    { 
        PC->bShowMouseCursor = false; 
        FInputModeGameOnly Mode; 
        PC->SetInputMode(Mode); 
    }
    RemoveFromParent();
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
        UpdateDeliveryUI();

        // if we've reached the required amount, wait briefly then advance to the next line
        // the short delay lets the player see the progress turn green before the dialogue continues
        if (QM->GetObjectiveProgress(CurrentQuestTag, CurrentObjectiveTag) >= CurrentAmountRequired)
        {
            FTimerHandle NextLineTimer;
            GetWorld()->GetTimerManager().SetTimer(NextLineTimer, this, &UDialogueWidget::DisplayNextLine, 1.0f, false);
        }
    }
}