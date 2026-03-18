// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dontdestroyonload/ICantCryGameInstance.h"
#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayTagAssetInterface.h"
#include "EngineUtils.h"
#include "ICC_GameMode.generated.h"

 /**
 * Main game mode of the game
 */
UCLASS(Blueprintable)
class ICANTCRY_API AICC_GameMode : public AGameMode
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	UPROPERTY()
	UICantCryGameInstance* GameInstance;

};
