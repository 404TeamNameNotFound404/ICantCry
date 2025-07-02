/*
 *  ICC Project
 *
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "ICC_PlayerController.generated.h"

/**
 * Class used to implement custom player controller
 */
UCLASS()
class ICANTCRY_API AICC_PlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	
	UFUNCTION(BlueprintCallable, Category = "Utility")
	static AICC_PlayerController* GetICCPlayerController(const UObject* WorldContextObject);
};
