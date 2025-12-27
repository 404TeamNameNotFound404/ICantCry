// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FieldSlot.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AFieldSlot : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFieldSlot();

	int32 GetId() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "FieldSlot")
	int32 Id;
};
