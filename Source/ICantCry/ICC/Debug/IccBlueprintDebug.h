// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "IccBlueprintDebug.generated.h"

/**
 * 
 */
UCLASS()
class ICANTCRY_API UIccBlueprintDebug : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Save the battlelog into a .log file inside Saved/Log directory
	 */
	UFUNCTION(BlueprintCallable, Category="ICC_Debug") static void EmitLog();
};
