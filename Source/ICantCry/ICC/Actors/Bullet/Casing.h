// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "Casing.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FCasing : public FTableRowBase
{
	GENERATED_BODY()
	
	FCasing() : Icon(nullptr) {}

	FString GetName() const;
	UTexture2D* GetIcon() const;
	int32 GetQuantity() const;

	void SetQuantity(const int32& NewQuantity);

	ECasingType& GetType();
	void SetType(const ECasingType& NewType);
	
	bool IsValid() const;
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Quantity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECasingType RequiredCasingType;
};
