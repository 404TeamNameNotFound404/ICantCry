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

	if (Queue.IsEmpty())
	{
		DebugHelper::LogMessage(15, FColor::Emerald, "Queue is invalid");
		return;
	}

	UBattleData* Bd = Instance->GetCurrentPlayer()->GetBattleData();

	TArray<UTextBlock*> Slots =
	{
		VictoryVisualizer->GetEssenceDrop1(),
		VictoryVisualizer->GetEssenceDrop2(),
		VictoryVisualizer->GetEssenceDrop3()
	};
	
	TMap<EEssenceType, FEssence> RewardMap;

	for (AICC_Actor* Entity : Queue)
	{
		if (!Entity || Entity->IsA(AICC_Player::StaticClass()))
		{
			continue;
		}

		AMob* Emotion = Cast<AMob>(Entity);

		if (!Emotion || Emotion->IsAlive())
		{
			continue;
		}

		FEssence DroppedEssence;

		switch (Emotion->GetMobType())
		{
		case MobAnger:
			DroppedEssence.Quantity = Bd->AngerDropQuantity;
			DroppedEssence.EssenceName = "Anger";
			DroppedEssence.EssenceType = EEssenceType::Anger;
			break;

		case MobShame:
			DroppedEssence.Quantity = Bd->ShameDropQuantity;
			DroppedEssence.EssenceName = "Shame";
			DroppedEssence.EssenceType = EEssenceType::Shame;
			break;

		case MobJoy:
			DroppedEssence.Quantity = Bd->JoyDropQuantity;
			DroppedEssence.EssenceName = "Joy";
			DroppedEssence.EssenceType = EEssenceType::Joy;
			break;

		case MobDisgust:
			DroppedEssence.Quantity = Bd->DisgustDropQuantity;
			DroppedEssence.EssenceName = "Disgust";
			DroppedEssence.EssenceType = EEssenceType::Disgust;
			break;

		case MobFear:
			DroppedEssence.Quantity = Bd->FearDropQuantity;
			DroppedEssence.EssenceName = "Fear";
			DroppedEssence.EssenceType = EEssenceType::Fear;
			break;

		case MobJealousy:
			DroppedEssence.Quantity = Bd->JealousyDropQuantity;
			DroppedEssence.EssenceName = "Jealousy";
			DroppedEssence.EssenceType = EEssenceType::Jealousy;
			break;

		case MobSadness:
			DroppedEssence.Quantity = Bd->SadnessDropQuantity;
			DroppedEssence.EssenceName = "Sadness";
			DroppedEssence.EssenceType = EEssenceType::Sadness;
			break;

		case MobAnxiety:
			DroppedEssence.Quantity = Bd->AnxietyDropQuantity;
			DroppedEssence.EssenceName = "Anxiety";
			DroppedEssence.EssenceType = EEssenceType::Anxiety;
			break;

		case MobCalm:
			DroppedEssence.Quantity = Bd->CalmDropQuantity;
			DroppedEssence.EssenceName = "Calm";
			DroppedEssence.EssenceType = EEssenceType::Calm;
			break;

		default:
			continue;
		}
		
		Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence(DroppedEssence);
		
		if (RewardMap.Contains(DroppedEssence.EssenceType))
		{
			RewardMap[DroppedEssence.EssenceType].Quantity += DroppedEssence.Quantity;
		}
		else
		{
			RewardMap.Add(DroppedEssence.EssenceType, DroppedEssence);
		}

		DebugHelper::LogMessage(
			18,
			FColor::Orange,
			"Dropped " + DroppedEssence.EssenceName);
	}
	
	int32 SlotIndex = 0;

	for (const TPair<EEssenceType, FEssence>& Pair : RewardMap)
	{
		if (SlotIndex >= Slots.Num())
		{
			break;
		}

		const FEssence& Essence = Pair.Value;

		Slots[SlotIndex]->SetText(
			FText::FromString(
				Essence.EssenceName +
				" x" +
				FString::FromInt(Essence.Quantity)));

		++SlotIndex;
	}
	
	VictoryVisualizer->GetEssenceDrop0()->SetText(
		FText::FromString(
			"Essence Types: " +
			FString::FromInt(RewardMap.Num())));
}
