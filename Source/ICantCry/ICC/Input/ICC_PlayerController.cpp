// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_PlayerController.h"


AICC_PlayerController* AICC_PlayerController::GetICCPlayerController(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = WorldContextObject->GetWorld();
	if (!World) return nullptr;

	return Cast<AICC_PlayerController>(UGameplayStatics::GetPlayerController(WorldContextObject, 0));
}