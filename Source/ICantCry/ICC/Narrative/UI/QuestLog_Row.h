// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Narrative/Data/QuestDefinition.h"
#include "QuestLog_Row.generated.h"

/**
 * CLASSE: UQuestLog_Row
 * DESCRIZIONE: Riga visuale del tuo ScrollBoxQuest.
 */
UCLASS()
class ICANTCRY_API UQuestLog_Row : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget)) 
	UTextBlock* QuestTitleText;

	void SetupRow(UQuestDefinition* Quest);
	
};
