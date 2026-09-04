// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IccEmotionAnimInstance.h"
#include "Components/ActorComponent.h"
#include "IccEmotionAnimDealer.generated.h"


class AMob;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UIccEmotionAnimDealer : public UActorComponent
{
	GENERATED_BODY()

public:
	UIccEmotionAnimDealer();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	UIccEmotionAnimInstance* AnimationInstance;
	
	UPROPERTY()
	AMob* Owner;
	
private:
	void Init();

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
