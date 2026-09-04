// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IccPlayerAnimInstance.h"
#include "IccAnimationDealer.generated.h"


class AICC_Player;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UIccAnimationDealer : public UActorComponent
{
	GENERATED_BODY()

public:
	UIccAnimationDealer();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	UIccPlayerAnimInstance* AnimationInstance;
	
	UPROPERTY()
	AICC_Player* Owner;
	
	UPROPERTY()
	UPawnMovementComponent* MoveCmp;
	
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	

	UIccPlayerAnimInstance* GetAnimationBlueprint();
	
	void SetInFight(const bool& InFight);
	void SetShooting(const bool& InShooting);
	
	
private:
	void Init();
};
