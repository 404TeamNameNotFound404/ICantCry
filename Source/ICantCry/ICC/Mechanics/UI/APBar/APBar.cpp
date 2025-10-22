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


void UAPBar::UpdateAp(const int32& Value, const int32& ApCurrent)
{
	SetPreviewAP(Value);          
	SetCurrentAP(ApCurrent);
	Update();     
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

	Init();
}
