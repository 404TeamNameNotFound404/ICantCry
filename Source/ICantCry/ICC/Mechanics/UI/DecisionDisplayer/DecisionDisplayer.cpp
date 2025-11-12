// Fill out your copyright notice in the Description page of Project Settings.


#include "DecisionDisplayer.h"

void UDecisionDisplayer::NativeConstruct()
{
	Super::NativeConstruct();
}

FText UDecisionDisplayer::UpdateDecisionText()
{
	return M_DecisionText;
}


void UDecisionDisplayer::SetDecisionText(const FText& Text) 
{
	M_DecisionText = Text;
}

void UDecisionDisplayer::Hide()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UDecisionDisplayer::Show()
{
	SetVisibility(ESlateVisibility::Visible);
}
