// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryVisualizer.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

void UVictoryVisualizer::NativeConstruct()
{
	Super::NativeConstruct();

	BackToWorld->OnClicked.AddDynamic(this, &UVictoryVisualizer::ReturnToWorld);
}

void UVictoryVisualizer::Setup(const TArray<AICC_Actor*>& Queue)
{
	EmotionsSlayed = {Em0, Em1, Em2};
	
	if (Queue.IsEmpty())
	{
		DebugHelper::LogMessage(10, FColor::Red, "Queue empty");
		return;
	}

	int32 TextIndex = 0;
	

	for (const AICC_Actor* Entity : Queue)
	{
		if (!Entity || Entity->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		if (TextIndex >= EmotionsSlayed.Num())
		{
			break; // Prevent out-of-bounds access
		}

		if (EmotionsSlayed[TextIndex])
		{
			EmotionsSlayed[TextIndex]->SetText(FText::FromString(Entity->GetName()));
		}

		++TextIndex;
	}

	MainText->SetText(FText::FromString("Victory"));
	ExpLabel->SetText(FText::FromString("Exp."));

	const int32 ExpGained = CalculateExp(Queue);
	ExpInt->SetText(FText::FromString(FString::FromInt(ExpGained)));
	EmotionSlayedLabel->SetText(FText::FromString("E.E"));
}


int32 UVictoryVisualizer::CalculateExp(const TArray<AICC_Actor*>& Queue)
{
	if (Queue.IsEmpty())
	{
		return 0;
	}

	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	int32 TotalExp = 0;

	for (AICC_Actor* Entity : Queue)
	{
		if (Entity->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		AMob* Emotion = Cast<AMob>(Entity);

		TotalExp += Emotion->GetData()->ExpGiven;
	}

	Instance->GetPlayerStats()->Experience += TotalExp;

	return TotalExp;
}

void UVictoryVisualizer::ReturnToWorld()
{
	// TODO LOAD THE SCENE AND Call 'RecreatePlayer' via UICantCryGameInstance
}
