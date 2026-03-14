// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterUI.h"
#include "../Actors/Player/ICC_Player.h"


void UCharacterUI::NativeConstruct()
{
    Super::NativeConstruct();

    // setup button name
    if(HealthStats) HealthStats->SetStatName("Health");
    if(AttackStats) AttackStats->SetStatName("Attack");
    if(DefenceStats) DefenceStats->SetStatName("Defence");
    if(SpeedStats) SpeedStats->SetStatName("Speed");

    // onclick
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

            // 2. FIX: Forza un aggiornamento immediato all'apertura del widget
            // Questo caricherà le missioni che il sistema ha mantenuto durante il cambio livello
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
    // UNBIND: Molto importante! Quando la UI sparisce, diciamo al Manager di non cercarci più.
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
    
    // bar % 
    ExpBar->SetPercent(Percentage);
    
    // update txt ExpCurrentTextBar

    if(ExpCurrentTextBar)
    {
        FString CurrentText = FString::Printf(TEXT("%.0f / %.0f"), CurrentExp, RequiredExp);

        ExpCurrentTextBar->SetText(FText::FromString(CurrentText));
    }


}


void UCharacterUI::OnQuestSystemUpdated()
{
    // // Chiamiamo la tua logica esistente di aggiornamento testi/liste
    // UpdateQuests();

    // // Se la missione che stavo guardando è stata appena rimossa, 
    // // svuota i testi a destra per non lasciare scritte "fantasma".
    // if (TextQuestTitle) TextQuestTitle->SetText(FText::GetEmpty());
    // if (TextQuestDescription) TextQuestDescription->SetText(FText::GetEmpty());
    // if (VerticalBoxObjectives) VerticalBoxObjectives->ClearChildren();

    UpdateQuests(); 

    // 2. AGGIORNAMENTO REAL-TIME DEI DETTAGLI (Il pezzo mancante!)
    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (QM && CurrentSelectedQuestTag.IsValid())
    {
        // Chiediamo al Manager i dati aggiornati per la missione che stiamo guardando
        int32 QuestIndex = QM->FindActiveQuestIndex(CurrentSelectedQuestTag);
        
        if (QuestIndex != INDEX_NONE)
        {
            // Se la missione è ancora attiva, passiamo i nuovi dati a DisplayQuestDetails
            // Questo eseguirà di nuovo il tuo ciclo FOR con i numeri aggiornati (1/3, 2/3...)
            DisplayQuestDetails(QM->GetActiveQuests()[QuestIndex]);
        }
        else
        {
            // Se la missione non è più attiva (magari è stata completata), puliamo la vista
            if (TextQuestTitle) TextQuestTitle->SetText(FText::GetEmpty());
            if (VerticalBoxObjectives) VerticalBoxObjectives->ClearChildren();
        }
    }
}


void UCharacterUI::ClearQuestDetails()
{
   // RESET VISIBILITÀ: Questo è fondamentale
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

    // 1. Controllo dei riferimenti del Widget
    if (!MainQuestScrollBox || !SideQuestScrollBox || !QuestEntryClass) 
    {
        if (!MainQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: MainQuestScrollBox è NULL!"));
        if (!SideQuestScrollBox) UE_LOG(LogTemp, Error, TEXT("DEBUG: SideQuestScrollBox è NULL!"));
        if (!QuestEntryClass)    UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestEntryClass non è assegnata nel Blueprint!"));
        return;
    }

    // 2. Pulizia liste
    MainQuestScrollBox->ClearChildren();
    SideQuestScrollBox->ClearChildren();

    // --- MODIFICA: Puliamo il pannello dei dettagli ---
    // Questo assicura che all'apertura o al refresh, i testi Title/Description/Objectives 
    // siano vuoti finché l'utente non clicca fisicamente su una riga.
    ClearQuestDetails();
   UE_LOG(LogTemp, Log, TEXT("DEBUG: Eseguito ClearQuestDetails() - Visibilità impostata a Collapsed"));

    // 3. Controllo del Quest Manager
    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    if (!QM) 
    {
        UE_LOG(LogTemp, Error, TEXT("DEBUG: QuestManagerSystem non trovato nel GameInstance!"));
        return;
    }

    // 4. Verifica quante quest sono attive nel Manager
    const TArray<FQuestProgress>& ActiveList = QM->GetActiveQuests();
    UE_LOG(LogTemp, Warning, TEXT("DEBUG: Il Manager riporta %d missioni attive."), ActiveList.Num());

    // 5. Ciclo di creazione dei widget
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

        // Creiamo il widget
        UQuestEntryWidget* NewEntry = CreateWidget<UQuestEntryWidget>(this, QuestEntryClass);
        if (NewEntry)
        {
            // Nota: SetupQuestEntry ora inizializza solo i dati interni e il Titolo della riga,
            // non chiama più DisplayQuestDetails automaticamente.
            NewEntry->SetupQuestEntry(ProgressData, this);

            // 6. DEBUG LOGICA TAGS
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
    if(HealthStatDescriptionTxt) HealthStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(AttackStatDescriptionTxt) AttackStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(DefenceStatDescriptionTxt) DefenceStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
    if(SpeedStatDescriptionTxt) SpeedStatDescriptionTxt->SetVisibility(ESlateVisibility::Hidden);
}

void UCharacterUI::HandleStatButtonClick(UStatsButtonWidget *ButtonToSelect, UTextBlock *DescriptionToShow)
{
    UStatsButtonWidget* AllButtons[] = {HealthStats, AttackStats, DefenceStats, SpeedStats};
    
    for (UStatsButtonWidget* Button : AllButtons)
    {
        if (Button)
        {
            Button->SetSelected(false);
        }
    }
    
    
    if (ButtonToSelect)  ButtonToSelect->SetSelected(true);
    
    // show description
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
    // 1. Controllo sicurezza iniziale
    if (!Details.QuestDef || !VerticalBoxObjectives || !ObjectiveRowClass) return;

    CurrentSelectedQuestTag = Details.QuestDef->QuestID;

    UQuestManagerSystem* QM = GetGameInstance()->GetSubsystem<UQuestManagerSystem>();
    bool bReadyToTurnIn = QM ? QM->AreObjectivesComplete(Details) : false;

    // 2. Aggiorna il Titolo e rendilo VISIBILE
    if (TextQuestTitle) 
    {
        TextQuestTitle->SetText(Details.QuestDef->Title);
        TextQuestTitle->SetVisibility(ESlateVisibility::Visible); // Lo rendiamo visibile qui
    }

    // 3. Aggiorna la Descrizione + Messaggio NPC e rendila VISIBILE
    if (TextQuestDescription)
    {
        TextQuestDescription->SetVisibility(ESlateVisibility::Visible); // Lo rendiamo visibile qui

        FText MainDesc = Details.QuestDef->QuestDescription;
        
        if (bReadyToTurnIn && Details.QuestDef->bRequiresNPCTurnIn)
        {
            FString TurnInString = FString::Printf(TEXT("%s\n\n*** OBIETTIVI COMPLETATI! Torna da %s per la ricompensa. ***"), 
                *MainDesc.ToString(), 
                *Details.QuestDef->TargetNPCName.ToString());
            
            TextQuestDescription->SetText(FText::FromString(TurnInString));
        }
        else
        {
            TextQuestDescription->SetText(MainDesc);
        }
    }

    // 4. LOGICA OBIETTIVI: Svuota, rendi VISIBILE e ricrea le righe
    VerticalBoxObjectives->ClearChildren();
    VerticalBoxObjectives->SetVisibility(ESlateVisibility::Visible); // Lo rendiamo visibile qui

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


