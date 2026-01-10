// Fill out your copyright notice in the Description page of Project Settings.

#include "../Actors/Player/ICC_Player.h"
#include "CharacterUI.h"

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

    //upd ui
    RefreshUI();

    
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

void UCharacterUI::UpdateQuests()
{   

    
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


