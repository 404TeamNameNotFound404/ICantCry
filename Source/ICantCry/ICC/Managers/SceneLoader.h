// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "SceneLoader.generated.h"

UCLASS()
class ICANTCRY_API ASceneLoader : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASceneLoader();

	// Change Scenes
	UFUNCTION(BlueprintCallable, Category = "Scene Loading")
	void LoadScene(FName SceneName);

	// UFUNCTION(BlueprintCallable, Category = "Scene Loading")
	// void OnSceneLoaded();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditAnywhere, Category = "Scene Loading") //( add scenes in editor )
	TArray<FName> AvailableScenes;

	FTimerHandle TimerHandle_LoadScene;

};
