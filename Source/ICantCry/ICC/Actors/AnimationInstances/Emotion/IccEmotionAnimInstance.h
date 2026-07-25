// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "IccEmotionAnimInstance.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UIccEmotionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "isAttacking?"))
	bool bIsAttacking;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(DisplayName = "isDeath?"))
	bool bIsDeath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MainBlending;
};
