// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterUI.h"
#include "../Actors/Player/ICC_Player.h"


void UCharacterUI::NativeConstruct()
{
    Super::NativeConstruct();

   // set up stat button display names
    if(HealthStats) HealthStats->SetStatName("Health");
    if(AttackStats) AttackStats->SetStatName("Attack");
    if(DefenceStats) DefenceStats->SetStatName("Defence");
    if(SpeedStats) SpeedStats->SetStatName("Speed");

    // bind stat button click events
    if (HealthStats) HealthStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnHealthStatsClicked);
    if (AttackStats) AttackStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnAttackStatsClicked);
    if (DefenceStats) DefenceStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnDefenceStatsClicked);
    if (SpeedStats) SpeedStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnSpeedStatsClicked);

    // subscribe to quest system updates and force an initial refresh
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UQuestManagerSystem* QM = GI->GetSubsystem<UQuestManagerSystem>())
        {
            QM->OnSystemUpdate.AddDynamic(this, &UCharacterUI::OnQuestSystemUpdated);
            UpdateQuests(); // force immediate update to show any quests that survived level transitions
        }
    }

    ClearQuestDetails();
    CurrentSelectedQuestTag = FGameplayTag::EmptyTag;
    RefreshUI();
}

void UCharacterUI::NativeDestruct()
{
    // unsubscribe from quest system updates when the widget is destroyed
    // this prevents crashes if the manager tries to call a destroyed widget
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UQuestManagerSystem* QM = GI->GetSubsystem<UQuestManagerSystem>())
        {
            QM->OnSystemUpdate.RemoveDynamic(this, &UCharacterUI::OnQuestSystemUpdated);
        }
    }

    Super::NativeDestruct();
}

void UCharacterUI::RefreshUI()
{
    UpdateStats();
    UpdateCharacterLevel();
    UpdateExpBar();
    UpdateQuests();
}

void UCharacterUI::UpdateStats()
{
    if(!Stats) return;

    if(HealthStats) HealthStats->UpdateStatValue(Stats->MaxHealth);
    if(AttackStats) AttackStats->UpdateStatValue(Stats->AttackPower);
    if(DefenceStats) DefenceStats->UpdateStatValue(Stats->DefencePower);
    if(SpeedStats) SpeedStats->UpdateStatValue(Stats->Priority);
}

void UCharacterUI::UpdateCharacterLevel()
{
    if(!Stats || !CharacterLVTop) return;

    FString LevelText =  FString::Printf(TEXT("LVL:  %d"), Stats->Level);
    CharacterLVTop->SetText(FText::FromString(LevelText));
}

void UCharacterUI::UpdateExpBar()
{
    if(!Stats || !ExpBar) return;

    AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn());
    if (!Player) return;

    float CurrentExp = Stats->Experience;
    float RequiredExp = Player->GetExpRequiredForNextLevel();
    float Percentage = Player->GetCurrentExpPercentage();
    
    ExpBar->SetPercent(Percentage);
    
    if(ExpCurrentTextBar)
    {
        FString CurrentText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, RequiredExp);
        ExpCurrentTextBar->SetText(FText::FromString(CurrentText));
    }
}

void UCharacterUI::OnQuestSystemUpdated()
{
    UpdateQuests(); 

    // real-time update of the currently selected quest details
    // this ensures objective counts update while the player is looking at them
    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (QM && CurrentSelectedQuestTag.IsValid())
    {
        int32 QuestIndex = QM->FindActiveQuestIndex(CurrentSelectedQuestTag);
        
        if (QuestIndex != INDEX_NONE)
        {
            // quest is still active, refresh its details with updated progress numbers
            DisplayQuestDetails(QM->GetActiveQuests()[QuestIndex]);
        }
        else
        {
            // quest was completed or removed, clear the details panel
            if (TextQuestTitle) TextQuestTitle->SetText(FText::GetEmpty());
            if (VerticalBoxObjectives) VerticalBoxObjectives->ClearChildren();
        }
    }
}

void UCharacterUI::ClearQuestDetails()
{
    // completely hide and clear all quest detail panels
    // this is called when opening the ui or when no quest is selected
    if (TextQuestTitle) 
    {
        TextQuestTitle->SetVisibility(ESlateVisibility::Collapsed);
        TextQuestTitle->SetText(FText::GetEmpty());
    }

    if (TextQuestDescription) 
    {
        TextQuestDescription->SetVisibility(ESlateVisibility::Collapsed);
        TextQuestDescription->SetText(FText::GetEmpty());
    }
    
    if (VerticalBoxObjectives)
    {
        VerticalBoxObjectives->ClearChildren();
        VerticalBoxObjectives->SetVisibility(ESlateVisibility::Collapsed);
    }

    CurrentSelectedQuestTag = FGameplayTag::EmptyTag;
}

void UCharacterUI::UpdateQuests()
{   
    UE_LOG(LogTemp, Warning, TEXT("=== DEBUG: Inizio UpdateQuotes ==="));

    // check that all required widget references and classes are valid
    if (!MainQuestScrollBox || !SideQuestScrollBox || !QuestEntryClass) 
    {
        if (!MainQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: MainQuestScrollBox is NULL!"));
        if (!SideQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: SideQuestScrollBox is NULL!"));
        if (!QuestEntryClass)    UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestEntryClass not assigned in blueprint!"));
        return;
    }

    // clear existing quest entries
    MainQuestScrollBox->ClearChildren();
    SideQuestScrollBox->ClearChildren();

    // clear details panel so it's empty until the player clicks a specific quest
    ClearQuestDetails();
    UE_LOG(LogTemp, Log, TEXT("DEBUG: ClearQuestDetails() executed - visibility set to collapsed"));

    // get the quest manager
    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (!QM) 
    {
        UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestManagerSystem not found in GameInstance!"));
        return;
    }

    // check how many quests are active
    const TArray<FQuestProgress>& ActiveList = QM->GetActiveQuests();
    UE_LOG(LogTemp, Warning, TEXT("DEBUG: Manager reports %d active quests."), ActiveList.Num());

    // create widget entries for each active quest
    for (int32 i = 0; i < ActiveList.Num(); i++)
    {
        const FQuestProgress& ProgressData = ActiveList[i];

        if (!ProgressData.QuestDef)
        {
            UE_LOG(LogTemp, Error, TEXT("DEBUG: Quest at index %d has NULL QuestDef!"), i);
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("DEBUG: Processing quest: %s (Type Tag: %s)"), 
            *ProgressData.QuestDef->Title.ToString(), 
            *ProgressData.QuestDef->QuestTypeTag.ToString());

        UQuestEntryWidget* NewEntry = CreateWidget<UQuestEntryWidget>(this, QuestEntryClass);
        if (NewEntry)
        {
            // setupquestentry only initializes internal data and the row title
            // it no longer calls displayquestdetails automatically
            NewEntry->SetupQuestEntry(ProgressData, this);

            UE_LOG(LogTemp, Log, TEXT("DEBUG: Tag comparison - Quest: %s | UI MainTag: %s"), 
                *ProgressData.QuestDef->QuestTypeTag.ToString(), 
                *MainTag.ToString());

            // sort quests into main or side based on their type tag
            if (ProgressData.QuestDef->QuestTypeTag.MatchesTag(MainTag))
            {
                UE_LOG(LogTemp, Warning, TEXT("DEBUG: Adding to MainQuestScrollBox"));
                MainQuestScrollBox->AddChild(NewEntry);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("DEBUG: Adding to SideQuestScrollBox"));
                SideQuestScrollBox->AddChild(NewEntry);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DEBUG: Failed to create QuestEntry widget for %s!"), *ProgressData.QuestDef->Title.ToString());
        }
    }
    
    UE_LOG(LogTemp, Warning, TEXT("=== DEBUG: End UpdateQuests ==="));
}

void UCharacterUI::HideAllStatDescriptions()
{
    if(HealthStatDescriptionTxt) HealthStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(AttackStatDescriptionTxt) AttackStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(DefenceStatDescriptionTxt) DefenceStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(SpeedStatDescriptionTxt) SpeedStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterUI::HandleStatButtonClick(UStatsButtonWidget *ButtonToSelect, UTextBlock *DescriptionToShow)
{
    UStatsButtonWidget* AllButtons[] = {HealthStats, AttackStats, DefenceStats, SpeedStats};
    
    // deselect all stat buttons first
    for (UStatsButtonWidget* Button : AllButtons)
    {
        if (Button)
        {
            Button->SetSelected(false);
        }
    }
    
    // select the clicked button
    if (ButtonToSelect)  ButtonToSelect->SetSelected(true);
    
    // show the corresponding description and hide others
    HideAllStatDescriptions();
    if (DescriptionToShow) DescriptionToShow->SetVisibility(ESlateVisibility::Visible);
}

void UCharacterUI::ShowStatDescription(UTextBlock *DescriptionToShow)
{
    HideAllStatDescriptions();
    if (DescriptionToShow) DescriptionToShow->SetVisibility(ESlateVisibility::Visible);
}

void UCharacterUI::DisplayQuestDetails(const FQuestProgress& Details)
{
    // safety checks
    if (!Details.QuestDef || !VerticalBoxObjectives || !ObjectiveRowClass) return;

    CurrentSelectedQuestTag = Details.QuestDef->QuestID;

    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    bool bReadyToTurnIn = QM ? QM->AreObjectivesComplete(Details) : false;

    // update title and make it visible
    if (TextQuestTitle) 
    {
        TextQuestTitle->SetText(Details.QuestDef->Title);
        TextQuestTitle->SetVisibility(ESlateVisibility::Visible);
    }

    // update description with optional turn-in message
    if (TextQuestDescription)
    {
        TextQuestDescription->SetVisibility(ESlateVisibility::Visible);

        FText MainDesc = Details.QuestDef->QuestDescription;
        
        if (bReadyToTurnIn && Details.QuestDef->bRequiresNPCTurnIn)
        {
            FString TurnInString = FString::Printf(TEXT("%s\n\n*** OBJECTIVES COMPLETE! Return to %s for reward. ***"), 
                *MainDesc.ToString(), 
                *Details.QuestDef->TargetNPCName.ToString());
            
            TextQuestDescription->SetText(FText::FromString(TurnInString));
        }
        else
        {
            TextQuestDescription->SetText(MainDesc);
        }
    }

    // clear and rebuild objective list with current progress
    VerticalBoxObjectives->ClearChildren();
    VerticalBoxObjectives->SetVisibility(ESlateVisibility::Visible);

    for (const FQuestObjective& Obj : Details.QuestDef->Objectives)
    {
        UUserWidget* ObjWidget = CreateWidget<UUserWidget>(this, ObjectiveRowClass);
        if (ObjWidget)
        {
            UTextBlock* ObjText = Cast<UTextBlock>(ObjWidget->GetWidgetFromName(TEXT("TextObjectiveName")));
            if (ObjText)
            {
                int32 CurrentCount = 0;
                if (Details.ObjectiveProgress.Contains(Obj.ObjectiveID))
                {
                    CurrentCount = Details.ObjectiveProgress[Obj.ObjectiveID];
                }

                FString GoalStr = FString::Printf(TEXT("%s: %d / %d"), 
                    *Obj.Description.ToString(), 
                    CurrentCount, 
                    Obj.RequiredCount);

                ObjText->SetText(FText::FromString(GoalStr));

                // turn text green when objective is complete
                if (CurrentCount >= Obj.RequiredCount)
                {
                    ObjText->SetColorAndOpacity(FLinearColor::Green);
                }
            }
            VerticalBoxObjectives->AddChild(ObjWidget);
        }
    }
}

void UCharacterUI::OnQuestSelected()
{
    // placeholder for future implementation
}

void UCharacterUI::OnHealthStatsClicked()
{
   HandleStatButtonClick(HealthStats, HealthStatDescriptionTxt);
}

void UCharacterUI::OnAttackStatsClicked()
{
    HandleStatButtonClick(AttackStats, AttackStatDescriptionTxt);
}

void UCharacterUI::OnDefenceStatsClicked()
{
    HandleStatButtonClick(DefenceStats, DefenceStatDescriptionTxt);
}

void UCharacterUI::OnSpeedStatsClicked()
{
   HandleStatButtonClick(SpeedStats, SpeedStatDescriptionTxt);
}


