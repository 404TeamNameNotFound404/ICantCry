// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FIccBattleDebuggerData.h"
#include "GameFramework/Actor.h"
#include "IccBattleDebugger.generated.h"

class UICantCryGameInstance;
class UBattleHUD;

UCLASS(Blueprintable)
class ICANTCRY_API AIccBattleDebugger : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AIccBattleDebugger();
	
	void FillInitialInfo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debugger", meta=(AllowPrivateAccess = "true"))
	bool bDisplay = false;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	
	UPROPERTY() UICantCryGameInstance* GameInstance;
	UPROPERTY() UBattleHUD* RefHud;
	
	UPROPERTY() TArray<FIccBattleDebuggerData> Infos;
	UPROPERTY() TArray<FIccDebuggerDecisionData> Decisions;
	UPROPERTY() TArray<FIccDebuggerTrackingData> Trackings;
	
	UPROPERTY() TArray<AICC_Actor*> Queue;
	
	void DisplayDebuggerData();
	void DisplayStats();
	void DisplayDecisionTables();
	void DisplayTrackers();
	void DisplayApFlows();
};
