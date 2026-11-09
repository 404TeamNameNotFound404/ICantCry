// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterUI.h"
#include "../Actors/Player/ICC_Player.h"


void UCharacterUI::NativeConstruct()
{
    Super::NativeConstruct();

    Instance = Cast<UICantCryGameInstance>(GetGameInstance());

    if (HealthStats) HealthStats->SetStatName("Health");
    if (AttackStats) AttackStats->SetStatName("Attack");
    if (DefenceStats) DefenceStats->SetStatName("Defence");
    if (SpeedStats) SpeedStats->SetStatName("Speed");

    if (HealthStats) HealthStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnHealthStatsClicked);
    if (AttackStats) AttackStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnAttackStatsClicked);
    if (DefenceStats) DefenceStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnDefenceStatsClicked);
    if (SpeedStats) SpeedStats->OnStatsButtonClicked.AddDynamic(this, &UCharacterUI::OnSpeedStatsClicked);


    if (UGameInstance* GI = GetGameInstance())
    {
        if (UQuestManagerSystem* QM = GI->GetSubsystem<UQuestManagerSystem>())
        {
            // 1. Ti iscrivi per i futuri aggiornamenti
            QM->OnSystemUpdate.AddDynamic(this, &UCharacterUI::OnQuestSystemUpdated);
            UpdateQuests();
        }
    }


    ClearQuestDetails();

    CurrentSelectedQuestTag = FGameplayTag::EmptyTag;

    //upd ui
    RefreshUI();


}




void UCharacterUI::NativeDestruct()
{
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
    if (!Stats) return;

    if (HealthStats) HealthStats->UpdateStatValue(Instance->GetRuntimeStats().CurrentHealth);
    if (AttackStats) AttackStats->UpdateStatValue(Stats->RuntimeStats.AttackPower);
    if (DefenceStats) DefenceStats->UpdateStatValue(Stats->RuntimeStats.DefencePower);
    if (SpeedStats) SpeedStats->UpdateStatValue(Stats->Priority);
}

void UCharacterUI::UpdateCharacterLevel()
{
    if (!Stats || !CharacterLVTop) return;
    const FString LevelText = FString::Printf(TEXT("LVL:  %d"), Instance->GetRuntimeStats().Level);
    CharacterLVTop->SetText(FText::FromString(LevelText));
}

void UCharacterUI::UpdateExpBar()
{
    if (!Stats || !ExpBar) return;

    const AICC_Player* Player = Cast<AICC_Player>(GetOwningPlayerPawn());

    if (!Player) return;

    const float CurrentExp = Instance->GetRuntimeStats().Experience;
    const float RequiredExp = Instance->GetRuntimeStats().MaxExperience; //Player->GetExpRequiredForNextLevel();
    const float Percentage = Player->GetCurrentExpPercentage();

    ExpBar->SetPercent(Percentage);

    if (ExpCurrentTextBar)
    {
        const FString CurrentText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, RequiredExp);

        ExpCurrentTextBar->SetText(FText::FromString(CurrentText));
    }
}


void UCharacterUI::OnQuestSystemUpdated()
{
    // UpdateQuests -> ClearQuestDetails resets the selection, so it must be saved before,
    // otherwise the open quest would close every time an objective progresses
    const FGameplayTag PreviouslySelectedTag = CurrentSelectedQuestTag;

    UpdateQuests();

    if (!PreviouslySelectedTag.IsValid()) return;

    UGameInstance* GI = GetGameInstance();
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
    if (!QM)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterUI::OnQuestSystemUpdated - QuestManagerSystem non trovato"));
        return;
    }

    // if the quest is no longer active (completed/removed) the panel is already cleared by UpdateQuests
    if (const int32 QuestIndex = QM->FindActiveQuestIndex(PreviouslySelectedTag); QuestIndex != INDEX_NONE)
    {
        DisplayQuestDetails(QM->GetActiveQuests()[QuestIndex]);
    }
}


void UCharacterUI::ToggleQuestDetails(FGameplayTag QuestID)
{
    if (!QuestID.IsValid())
    {
        UE_LOG(LogTemp, Warning, TEXT("CharacterUI::ToggleQuestDetails - QuestID non valido, click ignorato"));
        return;
    }

    // same quest clicked again -> close
    if (QuestID == CurrentSelectedQuestTag)
    {
        ClearQuestDetails();
        return;
    }

    UGameInstance* GI = GetGameInstance();
    UQuestManagerSystem* QM = GI ? GI->GetSubsystem<UQuestManagerSystem>() : nullptr;
    if (!QM)
    {
        UE_LOG(LogTemp, Error, TEXT("CharacterUI::ToggleQuestDetails - QuestManagerSystem non trovato"));
        return;
    }

    // the manager is the source of truth, the copy stored in the row could be older
    const int32 QuestIndex = QM->FindActiveQuestIndex(QuestID);
    if (QuestIndex == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("CharacterUI::ToggleQuestDetails - la quest %s non e' piu' attiva"), *QuestID.ToString());
        ClearQuestDetails();
        return;
    }

    DisplayQuestDetails(QM->GetActiveQuests()[QuestIndex]);
}


void UCharacterUI::ClearQuestDetails()
{
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
    UE_LOG(LogTemp, Warning, TEXT("=== DEBUG: Inizio UpdateQuests ==="));

    if (!MainQuestScrollBox || !SideQuestScrollBox || !QuestEntryClass)
    {
        if (!MainQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: MainQuestScrollBox è NULL!"));
        if (!SideQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: SideQuestScrollBox è NULL!"));
        if (!QuestEntryClass)    UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestEntryClass non è assegnata nel Blueprint!"));
        return;
    }

    MainQuestScrollBox->ClearChildren();
    SideQuestScrollBox->ClearChildren();

    ClearQuestDetails();
    UE_LOG(LogTemp, Log, TEXT("DEBUG: Eseguito ClearQuestDetails() - Visibilità impostata a Collapsed"));

    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (!QM)
    {
        UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestManagerSystem non trovato nel GameInstance!"));
        return;
    }

    const TArray<FQuestProgress>& ActiveList = QM->GetActiveQuests();
    UE_LOG(LogTemp, Warning, TEXT("DEBUG: Il Manager riporta %d missioni attive."), ActiveList.Num());

    for (int32 i = 0; i < ActiveList.Num(); i++)
    {
        const FQuestProgress& ProgressData = ActiveList[i];

        if (!ProgressData.QuestDef)
        {
            UE_LOG(LogTemp, Error, TEXT("DEBUG: Quest all'indice %d ha un QuestDef NULL!"), i);
            continue;
        }

        UE_LOG(LogTemp, Log, TEXT("DEBUG: Elaborazione missione: %s (Tag Tipo: %s)"),
            *ProgressData.QuestDef->Title.ToString(),
            *ProgressData.QuestDef->QuestTypeTag.ToString());

        UQuestEntryWidget* NewEntry = CreateWidget<UQuestEntryWidget>(this, QuestEntryClass);
        if (NewEntry)
        {
            NewEntry->SetupQuestEntry(ProgressData, this);

            UE_LOG(LogTemp, Log, TEXT("DEBUG: Comparazione Tag - Missione: %s | MainTag della UI: %s"),
                *ProgressData.QuestDef->QuestTypeTag.ToString(),
                *MainTag.ToString());

            if (ProgressData.QuestDef->QuestTypeTag.MatchesTag(MainTag))
            {
                UE_LOG(LogTemp, Warning, TEXT("DEBUG: Aggiunta a MainQuestScrollBox"));
                MainQuestScrollBox->AddChild(NewEntry);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("DEBUG: Aggiunta a SideQuestScrollBox"));
                SideQuestScrollBox->AddChild(NewEntry);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("DEBUG: Fallita la creazione del widget QuestEntry per %s!"), *ProgressData.QuestDef->Title.ToString());
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("=== DEBUG: Fine UpdateQuests ==="));

}



void UCharacterUI::HideAllStatDescriptions()
{
    if (HealthStatDescriptionTxt) HealthStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if (AttackStatDescriptionTxt) AttackStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if (DefenceStatDescriptionTxt) DefenceStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if (SpeedStatDescriptionTxt) SpeedStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterUI::HandleStatButtonClick(UStatsButtonWidget* ButtonToSelect, UTextBlock* DescriptionToShow)
{
    UStatsButtonWidget* AllButtons[] = { HealthStats, AttackStats, DefenceStats, SpeedStats };

    for (UStatsButtonWidget* Button : AllButtons)
    {
        if (Button)
        {
            Button->SetSelected(false);
        }
    }


    if (ButtonToSelect)  ButtonToSelect->SetSelected(true);

    HideAllStatDescriptions();

    if (DescriptionToShow) DescriptionToShow->SetVisibility(ESlateVisibility::Visible);

}

void UCharacterUI::ShowStatDescription(UTextBlock* DescriptionToShow)
{
    HideAllStatDescriptions();

    if (DescriptionToShow) DescriptionToShow->SetVisibility(ESlateVisibility::Visible);

}


void UCharacterUI::DisplayQuestDetails(const FQuestProgress& Details)
{
    if (!Details.QuestDef || !VerticalBoxObjectives || !ObjectiveRowClass)
    {
        if (!Details.QuestDef)       UE_LOG(LogTemp, Error, TEXT("CharacterUI::DisplayQuestDetails - QuestDef NULL"));
        if (!VerticalBoxObjectives)  UE_LOG(LogTemp, Error, TEXT("CharacterUI::DisplayQuestDetails - VerticalBoxObjectives NULL"));
        if (!ObjectiveRowClass)      UE_LOG(LogTemp, Error, TEXT("CharacterUI::DisplayQuestDetails - ObjectiveRowClass non assegnata nel Blueprint"));
        return;
    }

    CurrentSelectedQuestTag = Details.QuestDef->QuestID;

    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    const bool bReadyToTurnIn = QM ? QM->AreObjectivesComplete(Details) : false;

    if (TextQuestTitle)
    {
        TextQuestTitle->SetText(Details.QuestDef->Title);
        TextQuestTitle->SetVisibility(ESlateVisibility::Visible);
    }

    if (TextQuestDescription)
    {
        TextQuestDescription->SetVisibility(ESlateVisibility::Visible);
        TextQuestDescription->SetText(BuildQuestDescriptionText(Details, bReadyToTurnIn));
    }

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

                if (CurrentCount >= Obj.RequiredCount)
                {
                    ObjText->SetColorAndOpacity(FLinearColor::Green);
                }
            }
            VerticalBoxObjectives->AddChild(ObjWidget);
        }
    }
}


FText UCharacterUI::BuildQuestDescriptionText(const FQuestProgress& Details, const bool bReadyToTurnIn) const
{
    const UQuestDefinition* Def = Details.QuestDef;
    if (!Def) return FText::GetEmpty();

    if (!bReadyToTurnIn || !Def->bRequiresNPCTurnIn) return Def->QuestDescription;

    // per-quest message wins, the widget default is only a fallback
    const FText& Template = Def->TurnInMessage.IsEmpty() ? DefaultTurnInMessage : Def->TurnInMessage;
    if (Template.IsEmpty()) return Def->QuestDescription;

    if (Def->TargetNPCName.IsEmpty() && Template.ToString().Contains(TEXT("{NPCName}")))
    {
        UE_LOG(LogTemp, Warning, TEXT("CharacterUI - la quest %s usa {NPCName} ma TargetNPCName e' vuoto nel Data Asset"),
            *Def->QuestID.ToString());
    }

    // FText::Format instead of FString::Printf keeps the text localizable
    FFormatNamedArguments Args;
    Args.Add(TEXT("NPCName"), Def->TargetNPCName);
    const FText TurnInText = FText::Format(Template, Args);

    if (Def->QuestDescription.IsEmpty()) return TurnInText;

    return FText::Format(NSLOCTEXT("CharacterUI", "DescriptionWithTurnIn", "{0}\n\n{1}"), Def->QuestDescription, TurnInText);
}


void UCharacterUI::OnQuestSelected()
{

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


