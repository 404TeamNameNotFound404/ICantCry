// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Actors/MinigameSpawnables/Papers/Paper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "FieldSlot.h"
#include "ChallengeMinigame.generated.h"

class UPuzzleAssembled;

UCLASS(Blueprintable)
class ICANTCRY_API AChallengeMinigame : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AChallengeMinigame();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Challenge", meta=(AllowPrivateAccess=true))
	TSubclassOf<UUserWidget> TriggerWidget; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	UUserWidget* TriggerWidgetBlueprint = nullptr; // Trigger widget is the classic 'Press X to play the minigame'
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* Trigger;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	ACameraActor* CameraActor;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,  int32 OtherBodyIndex,  
				bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	bool GetIsInArea() const;

	void Exit();

	static AChallengeMinigame* Singleton;
	void PickPaper();
	APaper* GetCurrentPaper() const;

	TArray<TObjectPtr<AFieldSlot>> GetFieldSlots() const;
	void PlacePaperInSlot(APaper* Paper, AFieldSlot* Slot);
	AFieldSlot* FindSlot(const FVector& PaperLocation) const;
	void ReleasePaper();
	float GetSnapDistance() const;
	void SetMinigameStarted(const bool& Value);
	UBoxComponent* GetTriggerComponent() const;
	

private:
	UPROPERTY(VisibleAnywhere ,BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsMinigameStarted = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bInArea = false;

	UPROPERTY()
	UICantCryGameInstance* Instance = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	TArray<TObjectPtr<AFieldSlot>> TerrainSlots;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	FVector LocationOffset = {0, 0, 1};
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<APaper>> Papers;

	UPROPERTY()
	TMap<AFieldSlot*, APaper*> PaperMap;
	
	UPROPERTY()
	APaper* CurrentPaper = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	float SnapDistance = 80.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UPuzzleAssembled> VictoryWidgetClass;

	/**
	 * Stores the correct id sequence (es. 3 - 4 - 2 - 1) will mean that the papers must be placed at the correct fieldslots id and if both matches
	 * with the array position at that index the solution is found.
	 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Challenge", meta=(AllowPrivateAccess = "true")) TArray<int32> Solution;
	
	void InitSlots();
	
	void Resolve();
};
