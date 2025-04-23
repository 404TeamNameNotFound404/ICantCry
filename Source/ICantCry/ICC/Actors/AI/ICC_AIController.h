// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ICC_AIController.generated.h"

UCLASS()
class ICANTCRY_API AICC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit AICC_AIController(FObjectInitializer const& FObjectInitializer);

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
