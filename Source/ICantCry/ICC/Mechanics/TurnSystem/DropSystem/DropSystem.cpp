// Fill out your copyright notice in the Description page of Project Settings.
#include "DropSystem.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Actors/Bullet/Essence.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Actors/AI/Mob.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/Victory/VictoryVisualizer.h"


void UDropSystem::Drop(UWorld* World, UVictoryVisualizer* VictoryVisualizer, const TArray<AICC_Actor*>& Queue)
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(World->GetGameInstance());
	checkf(Instance, TEXT("Game instance null at UDropSystem::Drop"))

	FEssence Indifference;
	Indifference.Quantity = 1;
	Indifference.EssenceName = "Indifference";
	Indifference.EssenceType = EEssenceType::Indifference;

	Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence2(Indifference);
	VictoryVisualizer->GetEssenceDrop0()->SetText(FText::FromString(Indifference.EssenceName + " x" + FString::FromInt(Indifference.Quantity)));
	
	if (Queue.IsEmpty())
	{
		return;
	}

	int32 Index = 0;
	TArray<UTextBlock*> Slots = {VictoryVisualizer->GetEssenceDrop1(), VictoryVisualizer->GetEssenceDrop2(), VictoryVisualizer->GetEssenceDrop3()};

	for (AICC_Actor* Entity : Queue)
	{
		if (Entity->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		AMob* Emotion = Cast<AMob>(Entity);

		if (!Emotion || Index >= Slots.Num())
		{
			continue;
		}

		FEssence MobEssence;

		switch (Emotion->GetMobType())
		{
		case MobAnger:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Anger";
			MobEssence.EssenceType = EEssenceType::Anger;
			break;
		case MobShame:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Shame";
			MobEssence.EssenceType = EEssenceType::Shame;
			break;
		case MobJoy:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Joy";
			MobEssence.EssenceType = EEssenceType::Joy;
			break;
		case MobDisgust:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Disgust";
			MobEssence.EssenceType = EEssenceType::Disgust;
			break;
		case MobFear:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Fear";
			MobEssence.EssenceType = EEssenceType::Fear;
			break;
		case MobJealousy:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Jealousy";
			MobEssence.EssenceType = EEssenceType::Jealousy;
			break;
		case MobSadness:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Sadness";
			MobEssence.EssenceType = EEssenceType::Sadness;
			break;
		case MobAnxiety:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Anxiety";
			MobEssence.EssenceType = EEssenceType::Anxiety;
			break;
		case MobCalm:
			MobEssence.Quantity = 2;
			MobEssence.EssenceName = "Calm";
			MobEssence.EssenceType = EEssenceType::Calm;
			break;
		default:
			continue;
		}

		Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence2(MobEssence);
		Slots[Index]->SetText(FText::FromString(MobEssence.EssenceName + " x" + FString::FromInt(MobEssence.Quantity)));
		Index++;
	}

	DebugHelper::LogMessage(8, FColor::Orange, "Dropped"); 
}
