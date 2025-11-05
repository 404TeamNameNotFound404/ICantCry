// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "ICantCry/ICC/Actors/MinigameSpawnables/Papers/Paper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ChallengeMinigame.generated.h"

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
	

private:
	UPROPERTY(VisibleAnywhere ,BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsMinigameStarted = false;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bInArea = false;

	UPROPERTY()
	UICantCryGameInstance* Instance = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	TArray<TSoftObjectPtr<AActor>> TerrainSlots;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	FVector LocationOffset = {0, 0, 1};
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Challenge", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<APaper>> Papers;

	UPROPERTY()
	APaper* CurrentPaper = nullptr;

	void InitSlots();
};
