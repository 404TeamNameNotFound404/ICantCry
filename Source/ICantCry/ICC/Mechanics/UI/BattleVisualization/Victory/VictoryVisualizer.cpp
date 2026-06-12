// Fill out your copyright notice in the Description page of Project Settings.

#include "VictoryVisualizer.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Managers/SceneLoader.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UVictoryVisualizer::NativeConstruct()
{
	Super::NativeConstruct();

	BackToWorld->OnClicked.AddDynamic(this, &UVictoryVisualizer::ReturnToWorld);

	DropSystem = NewObject<UDropSystem>();
	checkf(DropSystem, TEXT("Drop system appears to be null"))

	EssenceDrop0->SetText(FText::FromString(""));
	EssenceDrop1->SetText(FText::FromString(""));
	EssenceDrop2->SetText(FText::FromString(""));
	EssenceDrop3->SetText(FText::FromString(""));
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
			break; 
		}

		if (EmotionsSlayed[TextIndex])
		{
			EmotionsSlayed[TextIndex]->SetText(FText::FromString(Entity->GetName()));
		}

		++TextIndex;
	}

	MainText->SetText(FText::FromString("Victory"));
	ExpLabel->SetText(FText::FromString("Exp."));
	
	EmotionSlayedLabel->SetText(FText::FromString("E.E"));
}

void UVictoryVisualizer::AfterBattle(const TArray<AICC_Actor*>& Queue)
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	ExpInt->SetText(FText::FromString(FString::FromInt(Instance->GetRuntimeStats().ExpSummary)));
	Instance->GetRuntimeStats().ExpSummary = 0.0f;
	DropSystem->Drop(GetWorld(), this, Queue);
	
	DebugHelper::LogMessage(10, FColor::Orange, "[VictoryVisualizer]: After battle reached!");
}

UTextBlock* UVictoryVisualizer::GetEssenceDrop0() const
{
	return EssenceDrop0;
}

UTextBlock* UVictoryVisualizer::GetEssenceDrop1() const
{
	return EssenceDrop1;
}

UTextBlock* UVictoryVisualizer::GetEssenceDrop2() const
{
	return EssenceDrop2;
}

UTextBlock* UVictoryVisualizer::GetEssenceDrop3() const
{
	return EssenceDrop3;
}

UButton* UVictoryVisualizer::GetButton() const
{
	return BackToWorld;
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

		const AMob* Emotion = Cast<AMob>(Entity);
		
		TotalExp += Emotion->GetData()->ExpGiven;
	}

	Instance->GetRuntimeStats().Experience += TotalExp;

	return TotalExp;
}

void UVictoryVisualizer::ReturnToWorld()
{
	UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());

	if (Instance)
	{
		if (Instance->GetCurrentPlayer() && Instance->GetCurrentPlayer()->GetBattleHUD())
		{
			Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->ExitBattle();
		}
		
		Instance->SetCanRecreatePlayer(true);
		
		FName MapToLoad = Instance->GetLastMainMapName();
		
		if (MapToLoad.IsNone())
		{
			MapToLoad = FName("EncounterTestCraft");
			DebugHelper::LogWarning("MapToLoad era vuota! Uso EncounterTest come fallback.");
		}
		
		USceneLoader::LoadSceneByName(GetWorld(), MapToLoad, true);

		DebugHelper::LogSuccess(FString::Printf(TEXT("ReturnToWorld: Tornando alla mappa %s"), *MapToLoad.ToString()));
	}
	else
	{
		DebugHelper::LogError("ReturnToWorld: GameInstance non trovato!");
	}
	
	DebugHelper::SaveLogToFile();
	DebugHelper::ClearAllLogs();
	
	// // TODO LOAD THE SCENE AND Call 'RecreatePlayer' via UICantCryGameInstance
	// UICantCryGameInstance* Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	//
	// Instance->GetCurrentPlayer()->GetBattleHUD()->GetBattleHandler()->GetTurnBasedSystem()->ExitBattle();
	// Instance->SetCanRecreatePlayer(true);
	// USceneLoader::LoadSceneByName(GetWorld(), "EncounterTestCraft", true);
	// DebugHelper::LogSuccess("ReturnToWorld");
	// DebugHelper::SaveLogToFile();
	// DebugHelper::ClearAllLogs();
}
