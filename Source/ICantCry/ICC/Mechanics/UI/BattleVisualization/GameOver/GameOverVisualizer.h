// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ICantCry/ICC/Mechanics/UI/BattleVisualization/BattleVisualizer.h"
#include "GameOverVisualizer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UGameOverVisualizer : public UBattleVisualizer
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DeathReason;

	UPROPERTY(meta=(BindWidget))
	UButton* Retry;

	UPROPERTY(meta=(BindWidget))
	UButton* LoadPreviousSave;

private:

	UFUNCTION() void RetryBattle();

	UFUNCTION() void LoadPrevious();
};
