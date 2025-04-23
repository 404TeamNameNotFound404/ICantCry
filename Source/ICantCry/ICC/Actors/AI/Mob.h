// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/UI/Actors/EnemyHealthBar/HealthBarWidgetCmp.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "Mob.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API AMob : public AICC_Actor
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMob();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHealthBarWidgetCmp* HealthBarComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Get EnemyData
	UEnemyDatas* GetData() const;

	UEnemyTactics* GetTactics() const;

	/**
	 * Debug only
	 * @note MUST BE REPLACED WITH THE ACTUAL SKIN
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic",  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	UBehaviorTree* GetBehaviorTree() const;

	void HighlightsSilhouette();
	void DisableSilhouette();

	void StartDefenceMinigame();

	static void DealDamage();

private:

	// Reference to Enemy Data Asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyDatas* EnemyData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyTactics* Moves;

	UPROPERTY()
	AMinigameHandler* MinigameHandler = nullptr;

	UPROPERTY()
	bool bEnableSilhouette = false;
	
	static FDamage Damage;
	
};
