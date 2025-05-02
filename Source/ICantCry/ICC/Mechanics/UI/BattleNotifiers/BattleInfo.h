// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BattleInfo.generated.h"
/**
 * Battle Info
 * It displays info during combat such as enemy attacks or skills
 * @note TBD
 */
UCLASS(Blueprintable)
class ICANTCRY_API UBattleInfo : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> InfoText;
	UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> TurnInfo;
	virtual void NativeConstruct() override;

public:
	UFUNCTION() void SetInfo(const FText& Text);
	UFUNCTION() void ClearInfo() const;

	UFUNCTION() void SetTurnInfo(const FText& Text);
	UFUNCTION() void ClearTurnInfo() const;
};
