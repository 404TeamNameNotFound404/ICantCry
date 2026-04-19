// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VendingMachineSlot.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Actors/Bullet/Casing.h"
#include "ICantCry/ICC/Actors/Bullet/Essence.h"
#include "ICantCry/ICC/Inventory/Recipe.h"
#include "VendingMachine.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AVendingMachine : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVendingMachine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="VendingMachine", meta=(AllowPrivateAccess = "true"))
	UWidgetComponent* VendingUi;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="VendingMachine", meta=(AllowPrivateAccess = "true"))
	UVendingMachineSlot* VendingMachineWidget;
	
};
