// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "ICantCry/ICC/Input/DataAssets/ICC_InputDataAsset.h"
#include "ICC_EnhancedInputCmp.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UICC_EnhancedInputCmp : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject, typename CallbackFunc>
		void BindNativeInputAction(UICC_InputDataAsset* InputConfig, const FGameplayTag& GameplayTag , ETriggerEvent
			TriggerEvent, UserObject* Target, CallbackFunc Callback);
	
};

template <class UserObject, typename CallbackFunc>
void UICC_EnhancedInputCmp::BindNativeInputAction(UICC_InputDataAsset* InputConfig, const FGameplayTag& GameplayTag,
	ETriggerEvent TriggerEvent, UserObject* Target, CallbackFunc Callback)
{
	checkf(InputConfig, TEXT("InputConfig must be a valid UDetectiveInputDataAsset"));

	if(UInputAction* InputAction = InputConfig->FindNativeInputByTag(GameplayTag))
	{
		BindAction(InputAction, TriggerEvent, Target, Callback);
	}
}
