// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/BattleVisualizer.h"
#include "Components/Button.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/DropSystem/DropSystem.h"
#include "VictoryVisualizer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UVictoryVisualizer : public UBattleVisualizer
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExpLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ExpInt;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EmotionSlayedLabel;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Em0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Em1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Em2;

	UPROPERTY(meta = (BindWidget))
	UButton* BackToWorld;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EssenceDrop0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EssenceDrop1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EssenceDrop2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* EssenceDrop3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UTextBlock*> EmotionsSlayed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UTextBlock*> ExpEmotionsSlayed;

public:
	void Setup(const TArray<AICC_Actor*>& Queue);

	void AfterBattle(const TArray<AICC_Actor*>& Queue);

	UTextBlock* GetEssenceDrop0() const;
	UTextBlock* GetEssenceDrop1() const;
	UTextBlock* GetEssenceDrop2() const;
	UTextBlock* GetEssenceDrop3() const;

private:
	int32 CalculateExp(const TArray<AICC_Actor*>& Queue);

	UFUNCTION() void ReturnToWorld();

	UPROPERTY()
	UDropSystem* DropSystem;
};
