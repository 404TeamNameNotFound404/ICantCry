// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/BattleVisualizer.h"
#include "Components/Button.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UTextBlock*> EmotionsSlayed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<UTextBlock*> ExpEmotionsSlayed;

public:
	void Setup(const TArray<AICC_Actor*>& Queue);


private:
	int32 CalculateExp(const TArray<AICC_Actor*>& Queue);

	UFUNCTION() void ReturnToWorld();
};
