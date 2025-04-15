// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MobHealthBar.h"
#include "HealthBarWidgetCmp.generated.h"

class AMob;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UHealthBarWidgetCmp : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthBarWidgetCmp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess=true), Category="UI")
	 TSubclassOf<UMobHealthBar> HealthBarWidget;

	UPROPERTY()
	UMobHealthBar* HealthBar = nullptr;

	UPROPERTY()
	AMob* Owner = nullptr;
};
