// Fill out your copyright notice in the Description page of Project Settings.
#include "BattleInfo.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UBattleInfo::NativeConstruct()
{
	ClearInfo();
}

void UBattleInfo::SetInfo(const FText& Text)
{
	if (!InfoText)
	{
		DebugHelper::LogError("Info is invalid");
		return;
	}
	
	InfoText->SetText(Text);
}

void UBattleInfo::ClearInfo() const
{
	if (!InfoText)
	{
		return;
	}
	
	InfoText->SetText(FText::FromString(""));
}
