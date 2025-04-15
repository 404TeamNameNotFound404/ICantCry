// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/UI/Actors/EnemyHealthBar/HealthBarWidgetCmp.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Get EnemyData
	UEnemyDatas* GetData() const;

	UEnemyTactics* GetTactics() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHealthBarWidgetCmp* HealthBarComponent;

	/**
	 * Debug only
	 * @note MUST BE REPLACED WITH THE ACTUAL SKIN
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic",  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	void HighlightsSilhouette();
	void DisableSilhouette();

private:

	// Reference to Enemy Data Asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyDatas* EnemyData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyTactics* Moves;
	

	UPROPERTY()
	bool bEnableSilhouette = false;
	
};
