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

	UBattleData* Bd = Instance->GetCurrentPlayer()->GetBattleData();

	FEssence Indifference;
	// Indifference.Quantity = 1;
	Indifference.EssenceName = "Indifference";
	Indifference.EssenceType = EEssenceType::Indifference;
	Indifference.Quantity = 0;
	//
	// Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence2(Indifference);
	// VictoryVisualizer->GetEssenceDrop0()->SetText(FText::FromString(Indifference.EssenceName + " x" + FString::FromInt(Indifference.Quantity)));
	
	if (Queue.IsEmpty())
	{
		return;
	}

	int32 Index = 0;
	TArray<UTextBlock*> Slots = {VictoryVisualizer->GetEssenceDrop1(), VictoryVisualizer->GetEssenceDrop2(), VictoryVisualizer->GetEssenceDrop3()};

	FEssence MobEssence;

	TArray<FString> EnemySlayedSlot;
	TSet<EEssenceType> PrintedTypes;
	
	for (AICC_Actor* Entity : Queue)
	{
		if (Entity->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		AMob* Emotion = Cast<AMob>(Entity);

		if (!Emotion || Emotion->IsAlive() || Index >= Slots.Num())
		{
			continue;
		}
		
		// FEssence MobEssence;

		switch (Emotion->GetMobType())
		{
		case MobAnger:
			MobEssence.Quantity = Bd->AngerDropQuantity;
			MobEssence.EssenceName = "Anger";
			MobEssence.EssenceType = EEssenceType::Anger;
			Indifference.Quantity++;
			break;
		case MobShame:
			MobEssence.Quantity = Bd->ShameDropQuantity;
			MobEssence.EssenceName = "Shame";
			MobEssence.EssenceType = EEssenceType::Shame;
			Indifference.Quantity++;
			break;
		case MobJoy:
			MobEssence.Quantity = Bd->JoyDropQuantity;
			MobEssence.EssenceName = "Joy";
			MobEssence.EssenceType = EEssenceType::Joy;
			Indifference.Quantity++;
			break;
		case MobDisgust:
			MobEssence.Quantity = Bd->DisgustDropQuantity;
			MobEssence.EssenceName = "Disgust";
			MobEssence.EssenceType = EEssenceType::Disgust;
			Indifference.Quantity++;
			break;
		case MobFear:
			MobEssence.Quantity = Bd->FearDropQuantity;
			MobEssence.EssenceName = "Fear";
			MobEssence.EssenceType = EEssenceType::Fear;
			Indifference.Quantity++;
			break;
		case MobJealousy:
			MobEssence.Quantity = Bd->JealousyDropQuantity;
			MobEssence.EssenceName = "Jealousy";
			MobEssence.EssenceType = EEssenceType::Jealousy;
			Indifference.Quantity++;
			break;
		case MobSadness:
			MobEssence.Quantity = Bd->SadnessDropQuantity;
			MobEssence.EssenceName = "Sadness";
			MobEssence.EssenceType = EEssenceType::Sadness;
			Indifference.Quantity++;
			break;
		case MobAnxiety:
			MobEssence.Quantity = Bd->AnxietyDropQuantity;
			MobEssence.EssenceName = "Anxiety";
			MobEssence.EssenceType = EEssenceType::Anxiety;
			Indifference.Quantity++;
			break;
		case MobCalm:
			MobEssence.Quantity = Bd->CalmDropQuantity;
			MobEssence.EssenceName = "Calm";
			MobEssence.EssenceType = EEssenceType::Calm;
			Indifference.Quantity++;
			break;
		default:
			continue;
		}

		Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence2(MobEssence);
		Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence2(Indifference);

		if (!PrintedTypes.Contains(MobEssence.EssenceType))
		{
			FString DisplayText = MobEssence.EssenceName + " x" + FString::FromInt(MobEssence.Quantity);
			EnemySlayedSlot.Add(DisplayText);
			PrintedTypes.Add(MobEssence.EssenceType);
		}
		// Slots[Index]->SetText(FText::FromString(MobEssence.EssenceName + " x" + FString::FromInt(MobEssence.Quantity)));
		Index++;
		DebugHelper::LogMessage(8, FColor::Orange, "Dropped " + MobEssence.EssenceName); 
	}

	for (int32 i = 0; i < EnemySlayedSlot.Num() && i < Slots.Num(); ++i)
	{
		Slots[i]->SetText(FText::FromString(EnemySlayedSlot[i]));
	}
	
	VictoryVisualizer->GetEssenceDrop0()->SetText(FText::FromString(Indifference.EssenceName + " x" + FString::FromInt(Indifference.Quantity)));
	
}
