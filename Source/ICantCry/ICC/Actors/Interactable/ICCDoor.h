// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/LevelSequence/Public/LevelSequence.h"
#include "Runtime/LevelSequence/Public/LevelSequencePlayer.h"
#include "Runtime/LevelSequence/Public/LevelSequenceActor.h"
#include "GameplayTagContainer.h"
#include "ICCDoor.generated.h"


class UBoxComponent;
class UDialogueAsset;
class USoundBase;
class UAudioComponent;
class UICantCryGameInstance;
class AICC_Player;


UCLASS(Blueprintable)
class ICANTCRY_API AICCDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AICCDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	ULevelSequence* FadeIn;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponent;


	UPROPERTY() ALevelSequenceActor* SeqActor = nullptr;



	// --- LOCK ---
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock", meta = (AllowPrivateAccess = "true"))
	bool bIsLocked = false;

	/** Required key tag (must be in GameInstance inventory) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock", meta = (EditCondition = "bIsLocked", AllowPrivateAccess = "true"))
	FGameplayTag KeyItemTag;

	/** Player barks when he does NOT have the key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock", meta = (EditCondition = "bIsLocked", AllowPrivateAccess = "true"))
	UDialogueAsset* LockedBark;

	/** Sound of the lock when you open it with the key */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Door|Lock", meta = (EditCondition = "bIsLocked", AllowPrivateAccess = "true"))
	USoundBase* UnlockSound;

	UPROPERTY()
	UAudioComponent* UnlockAudioComponent = nullptr;

	UPROPERTY()
	bool bPlayerInside = false;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
