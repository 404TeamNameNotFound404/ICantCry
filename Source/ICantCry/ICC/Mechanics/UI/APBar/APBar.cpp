// Fill out your copyright notice in the Description page of Project Settings.


#include "APBar.h"

#include "ICantCry/ICC/Debug/DebugHelper.h"

void UAPBar::Init()
{
	CurrentAp = FMath::Clamp(2, 0, MaxAp);
	PreviewAp = 0;
	Update();
}

void UAPBar::IncreaseAP(const int32& Value)
{
	CurrentAp = FMath::Clamp(CurrentAp + Value, 0, MaxAp);
	Update();
}

void UAPBar::DecreaseAP(const int32& Value)
{
	CurrentAp = FMath::Clamp(CurrentAp - Value, 0, MaxAp);
	DebugHelper::LogWarning("AP decreased now " + FString::FromInt(CurrentAp));
	DebugHelper::AddMessageToLog("AP decreased now " + FString::FromInt(CurrentAp));
	Update();
}

void UAPBar::Update()
{
	//const int32 VisualPreview = FMath::Clamp(PreviewAp, 0, CurrentAp); 

	for (int32 i = 0; i < ApBar.Num(); ++i)
	{
		if (!ApBar[i]) continue;

		if (i < PreviewAp)
		{
			ApBar[i]->SetColorAndOpacity(WastedColor); 
		}
		else if (i < CurrentAp)
		{
			ApBar[i]->SetColorAndOpacity(ApColor); 
		}
		else
		{
			ApBar[i]->SetColorAndOpacity(WastedColor); 
		}
	}
}

void UAPBar::UpdateHighlights()
{
	SlotIndex = (SlotIndex + 1) % SlotsOutline.Num();
	SlotsOutline[SlotIndex]->SetColorAndOpacity(SlotOutlineColor);
}

void UAPBar::UpdateHighlights(const int32& Value)
{
	for (UImage* Img : SlotsOutline)
	{
		if (Img)
			Img->SetColorAndOpacity(FLinearColor::White);
	}

	// Highlight the first N active slots
	for (int32 i = 0; i < Value && i < SlotsOutline.Num(); ++i)
	{
		SlotsOutline[i]->SetColorAndOpacity(SlotOutlineColor);
	}
}

void UAPBar::ResetHighlightOf(const int32& Value)
{
	if (const int32 IndexToReset = Value - 1; SlotsOutline.IsValidIndex(IndexToReset))
	{
		SlotsOutline[IndexToReset]->SetColorAndOpacity(FLinearColor::White);
	}
}

void UAPBar::UpdateAp(const int32& Value, const int32& ApCurrent)
{
	SetPreviewAP(Value);          
	SetCurrentAP(ApCurrent);
	Update();     
}

void UAPBar::ClearAt(const int32& Value)
{
	for (int32 i = 0; i < Value && i < SlotsOutline.Num(); ++i)
	{
		SlotsOutline[i]->SetColorAndOpacity(FLinearColor::White);
	}
}

void UAPBar::ClearAll()
{
	for (UImage* Img: SlotsOutline)
	{
		Img->SetColorAndOpacity(DefaultColor);
	}
}

void UAPBar::SetCurrentAP(const int32& Value)
{
	CurrentAp = FMath::Clamp(Value, 0, MaxAp);
	Update();
}

void UAPBar::SetPreviewAP(const int32& Value)
{
	PreviewAp = FMath::Clamp(Value, 0, MaxAp);
	Update();
	DebugHelper::LogWarning("Ap wasted " + FString::FromInt(CurrentAp));
}

void UAPBar::NativeConstruct()
{
	Super::NativeConstruct();

	ApBar = {Slot0, Slot1, Slot2, Slot3};
	SlotsOutline = {ApHighlight_0, ApHighlight_1, ApHighlight_2, ApHighlight_3};

	for (UImage* Img : SlotsOutline)
	{
		Img->SetColorAndOpacity(FColor::White);
	}

	Init();
}
