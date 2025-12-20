// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Mechanics/ChallengeMinigames/FieldSlot.h"
#include "Paper.generated.h"

class AChallengeMinigame;

UCLASS(Blueprintable, Abstract)
class ICANTCRY_API APaper : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APaper();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	ACameraActor* MinigameCamera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	UTexture2D* Texture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true")) UStaticMeshComponent* PaperMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess = "true")) UMaterialInstanceDynamic* PaperMaterial;

	UPROPERTY() bool bLifted = false;
	UPROPERTY() bool bIsSwapped = false;
	UPROPERTY() bool bIsBeingDragged = false;
	UPROPERTY() FVector StartLocation;
	UPROPERTY() FVector CurrentLocation;

	UPROPERTY() TObjectPtr<AFieldSlot> CurrentSlot;
	UPROPERTY() TObjectPtr<AFieldSlot> HoveredSlot;
	UPROPERTY() TObjectPtr<AChallengeMinigame> Minigame;
	
	/**
	 * Id defines the piece order (es. 1 stands for first piece)
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	int32 Id;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	int32 SnapDistance = 80.0f;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Lift(const FVector& LiftLocation);
	virtual void Release();

	FVector GetStartLocation() const;
	FVector GetCurrentLocation() const;
	bool GetIsLifted() const;
	bool GetIsSwapped() const;
	void SetIsDragged(const bool& Value);

	void SetMinigameOwner(AChallengeMinigame* Value);
	void SetCurrentSlot(AFieldSlot* Value);
	AFieldSlot* GetCurrentSlot() const;
	void SetTexture(UTexture2D* Value);
	UTexture2D* GetTexture() const;
	int32 GetId() const;
};
