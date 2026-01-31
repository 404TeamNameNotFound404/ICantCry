// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsButtonWidget.h"

void UStatsButtonWidget::NativeConstruct()
{
    if(StatButton) StatButton->OnClicked.AddDynamic(this, &UStatsButtonWidget::OnButtonClicked);

    if (SelectionHighlight) SelectionHighlight->SetVisibility(ESlateVisibility::Collapsed);
  
}

void UStatsButtonWidget::UpdateStatValue(int32 Value)
{
    
    if(StatValueTxt) 
    {

        FString ValueString = FText::AsNumber(Value).ToString();

        StatValueTxt->SetText(FText::FromString(ValueString));
    }
}

void UStatsButtonWidget::SetStatName(FString Name)
{   
    if(StatNameTxt)
    {
       StatNameTxt->SetText(FText::FromString(Name));
    }

}

void UStatsButtonWidget::SetSelected(bool bSelected)
{
    bIsSelected = bSelected;

    if (SelectionHighlight) 
    {
        if (bIsSelected)
        {
            SelectionHighlight->SetVisibility(ESlateVisibility::Visible);
        }
        else
        {
            SelectionHighlight->SetVisibility(ESlateVisibility::Collapsed);
        }
    }

}

bool UStatsButtonWidget::IsSelected() const
{
    return bIsSelected;
}

void UStatsButtonWidget::OnButtonClicked()
{
    OnStatsButtonClicked.Broadcast();
}
