// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "IccPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UIccPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "inFight?"))
	bool bInFight;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "Shooting?"))
	bool bShooting;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LocomotionBlending;
	
	
};
