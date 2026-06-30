// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameUserWidget.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "DefenceMinigame.generated.h"


/**
 * Defence Minigame 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UDefenceMinigame : public UMinigameUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	/**
* The slider that will move from right to left
*/
	UPROPERTY(meta = (BindWidget))
	UImage* Slider;

	/**
	 * The last border to be hit
	 */
	UPROPERTY(meta = (BindWidget))
	UImage* LeftSeparator;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Icon;
	
public:
	virtual void MoveSlider(const FVector2D& Position) override;
	virtual EMinigameThreshold CheckBar() override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	float Speed = 1.f;
	
	float EndThreshold = 660.0f; 

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minigame", meta=(AllowPrivateAccess = "true"))
	float BadThreshold = 600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minigame", meta=(AllowPrivateAccess = "true"))
	float GoodThreshold = 620;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Minigame", meta=(AllowPrivateAccess = "true"))
	float PerfectThreshold = 640;

	UPROPERTY()
	UICantCryGameInstance* Instance = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* WorseScore;
	UPROPERTY(meta = (BindWidget))
	UImage* MediumScore;
	UPROPERTY(meta = (BindWidget))
	UImage* PerfectScore;
	
	virtual void HandleScore() override;
	
	FString GetThresholdName(const EMinigameThreshold& T) const;
};
