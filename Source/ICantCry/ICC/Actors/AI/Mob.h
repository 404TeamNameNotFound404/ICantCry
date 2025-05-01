// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICC_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/UI/Actors/EnemyHealthBar/HealthBarWidgetCmp.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "ICantCry/ICC/Actors/AI/Memory/EmotionMemory.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "Mob.generated.h"

class ABattleHandler;

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

	/**
	 * Emotions shape
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Character Shape", meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* Shape;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHealthBarWidgetCmp* HealthBarComponent;

	// Reference to Enemy Data Asset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyDatas* EnemyData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyTactics* Moves;

	UPROPERTY()
	AICC_AIController* AIController;
	

	//----------------

	UPROPERTY()
	FEmotionMemory Memory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

	/**
	 * Turn it on if the AI can cure the other mobs
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Behaviors", meta = (AllowPrivateAccess = "true"))
	bool bIsHealer;

	
	UPROPERTY()
	bool bSelf;

	UPROPERTY()
	bool bIsBuffed;

	UPROPERTY()
	bool bIsDebuffed;

	UPROPERTY()
	AICC_Player* DebugPlayer = nullptr;

	UPROPERTY()
	bool bEnvyBurned;

	UPROPERTY()
	bool bIsAshamedState;

	UPROPERTY()
	bool bIsLow;

	UPROPERTY()
	bool bFreezedUp;

	UPROPERTY()
	bool bAttacked;

	UPROPERTY()
	int Bt_Id;

	UPROPERTY()
	int AI_Id;
	
	//------

	static bool bStopTree;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/**
	 * Enemy Data
	 * @return Enemy Data
	 */
	UEnemyDatas* GetData() const;

	/**
	 * AI moves 
	 * @return Get the AIMoves
	 */
	UEnemyTactics* GetTactics() const;

	/**
	 * Debug only
	 * @note MUST BE REPLACED WITH THE ACTUAL SKIN
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic",  meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	/**
	 * Get The current behavior tree
	 * @return Selected Mob behavior tree
	 */
	UBehaviorTree* GetBehaviorTree() const;

	FEmotionMemory GetAIMemory() const;

	/**
	 * Highlights the silhouette during AI turn
	 */
	void HighlightsSilhouette();
	/**
	 * Disable the Silhouette
	 */
	void DisableSilhouette();

	void StartDefenceMinigame();
	virtual void Heal(const float& RestoredHealth);

	static void DealDamage();

	void SetIsBusy(const bool& Value);
	bool IsBusy() const;

	void PlayTurn();

	void PlaySecondTurn();
	
	void EndTurn();
	static bool MinigameEnded;


	/**
	 * Check if the minigame has started
	 * @return true if started
	 */
	static bool IsMinigameStarted();

	/**
	 * Set the minigame bool value
	 * @note this function is used for behavior decorator attack and retreat
	 * @param Value true or false
	 */
	static void SetMinigameStarted(const bool& Value);

	/**
	 * Get the battle handler
	 * @return Current Battle Handler
	 */
	ABattleHandler* GetBattleHandler() const;

	/**
	 * Get the health bar inside the widget component if not null
	 * @return valid health-bar
	 */
	UMobHealthBar* GetHealthBar() const;

	/**
	 * Check if current Emotion can heal or not
	 * @return true if healer
	 */
	bool IsHealer() const;

	void SetIsBuffed(const bool& Value);
	
	void SetIsDebuffed(const bool& Value);

	void SetIsAshamedState(const bool& Value);
	
	void SetIsLow(const bool& Value);
	
	void SetIsFreezedUp(const bool& Value);

	void SetIsAttacked(const bool& Value);

	void SetIsEnvyBurned(const bool& Value);

	void SetTreeId(const int& Value);

	bool GetIsIsBuffed() const;
	bool GetIsIsDebuffed() const;
	bool GetIsIsAshamedState() const;
	bool GetIsIsLow() const;
	bool GetIsIsFreezedUp() const;
	bool GetIsIsAttacked() const;
	bool GetIsIsEnvyBurned() const;
	int GetTreeId() const;
	int GetAIId() const;

private:

	UPROPERTY()
	AMinigameHandler* MinigameHandler = nullptr;

	UPROPERTY()
	bool bEnableSilhouette = false;
	
	static FDamage Damage;

	/**
	 * Check if AI is performing attack
	 */
	UPROPERTY()
	bool bBusy = false;

	UPROPERTY()
	UICantCryGameInstance* Instance;

	UPROPERTY()
	ABattleHandler* BattleHandler = nullptr;

	UPROPERTY()
	FTimerHandle BehaviorTreeTimerHandle;
	
	static bool bMinigameHasStarted;
};
