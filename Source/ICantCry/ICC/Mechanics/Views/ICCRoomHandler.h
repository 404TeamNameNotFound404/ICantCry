// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ICCRoomHandler.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoomChanged, AActor*);

UCLASS(Blueprintable)
class ICANTCRY_API AICCRoomHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AICCRoomHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Room Manager", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AActor>> Rooms;
	
	void SetRoomVisible(AActor* Room, const bool& bVisible);
	void HideOthers(AActor* Other);
	
	bool HideAll();
	
public:
	TArray<TObjectPtr<AActor>> GetRooms();
	FOnRoomChanged OnRoomChanged;
	
	void InitEvent();
};
