// Fill out your copyright notice in the Description page of Project Settings.


#include "QuestLog_Row.h"

void UQuestLog_Row::SetupRow(UQuestDefinition* Quest)
{
	if (Quest && QuestTitleText)
	{
		QuestTitleText->SetText(Quest->Title);
	}
}