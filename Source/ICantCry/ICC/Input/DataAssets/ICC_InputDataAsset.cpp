// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_InputDataAsset.h"


UInputAction* UICC_InputDataAsset::FindNativeInputByTag(const FGameplayTag& InputTag) const
{
	for(const FDetectiveInputConfig& config : InputConfigs)
	{
		if(config.InputTag == InputTag && config.InputAction)
		{
			return config.InputAction;
		}
	}

	return nullptr;
}