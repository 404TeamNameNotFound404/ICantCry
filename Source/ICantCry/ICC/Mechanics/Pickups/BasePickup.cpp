// Fill out your copyright notice in the Description page of Project Settings.
#include "BasePickup.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "Kismet/GameplayStatics.h"

void ABasePickup::Collect(AICC_Player* Player)
{
	if (!Player) return;
   
    if ( UGameInstance* GI = Cast<UGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); GI)
    {
        UQuestManagerSystem* QuestManager = GI->GetSubsystem<UQuestManagerSystem>();
        if (QuestManager)
        {
            QuestManager->UpdateObjectiveProgress(TargetQuestTag, TargetObjectiveTag, AmountToAdd);
            DebugHelper::LogSuccess(FString::Printf(TEXT("Sasso Raccolto! Obiettivo: %s"), *TargetObjectiveTag.ToString()));
        }
    }
    
    Destroy();
}
