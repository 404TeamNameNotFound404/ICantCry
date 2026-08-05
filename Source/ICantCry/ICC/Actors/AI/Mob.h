// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ICC_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "DecisionMaker/DecisionTable.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyDatas.h"
#include "ICantCry/ICC/Mechanics/Core/Data/EnemyTactics.h"
#include "ICantCry/ICC/Mechanics/UI/Actors/EnemyHealthBar/HealthBarWidgetCmp.h"
#include "ICantCry/ICC/Mechanics/Core/Minigame/MinigameHandler.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "ICantCry/ICC/Actors/AI/Memory/EmotionMemory.h"
#include "ICantCry/ICC/Mechanics/Core/Data/BattleData.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Actors/AI/Emotions/EmotionStats/FEmotionStat.h"
#include "ICantCry/ICC/Actors/AI/MobType.h"
#include "ICantCry/ICC/Actors/AnimationInstances/Emotion/IccEmotionAnimDealer.h"
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
	UBattleData* BattleData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enemy Data",  meta = (AllowPrivateAccess = "true"))
	UEnemyTactics* Moves;

	UPROPERTY()
	AICC_AIController* AIController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<EMobType> Type;
	
	UPROPERTY()
	UIccEmotionAnimDealer* AnimationDealer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay", meta = (AllowPrivateAccess = "true"))
	float LocomotionSpeed = 600.f;
	
	//----------------

	UPROPERTY()
	FEmotionMemory Memory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors", meta = (AllowPrivateAccess = "true"))
	UBehaviorTree* Tree;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Battle", meta = (AllowPrivateAccess = "true"))
	FEmotionStat Stats;

	/**
	 * Variable used to determinate if Emotion is 'Joy'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsHealer = false;

	/**
	 * Used to determinate if Emotion is 'Anger'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bEAnger = false;

	/**
	 * Used to determinate if Emotion is 'Fear'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsEFear = false;

	/**
	 * Used to determinate is Emotion is 'Disgust'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsEDisgust = false;

	/**
	 * Used to determinate if Emotion is 'Sadness'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsESadness = false;

	/**
	 * Used to determinate if Emotion is 'Anxiety'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsEAnxiety = false;

	/**
	 * Used to determinate if Emotion is 'Calm'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsECalm = false;

	/**
	 * Used to determinate if Emotion is 'Jealousy'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsEJealousy = false;

	/**
	 * Used to determinate if Emotion is 'Shame'
	 */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Species", meta = (AllowPrivateAccess = "true"))
	bool bIsEShame = false;
	
	UPROPERTY()
	bool bSelf;

	UPROPERTY()
	bool bIsBuffedAtk;

	UPROPERTY()
	bool bIsDebuffedDefence;

	UPROPERTY()
	bool bIsAttackDebuffed;

	UPROPERTY()
	AICC_Player* DebugPlayer = nullptr;

	UPROPERTY()
	bool bEnvyBurned;

	UPROPERTY()
	bool bIsAshamedState;

	UPROPERTY()
	bool bDebuffShield;

	UPROPERTY()
	bool bDebuffOtherShield;

	UPROPERTY()
	bool bHeal;

	UPROPERTY()
	bool bHealOther;

	UPROPERTY()
	bool bBuffDefence;

	UPROPERTY()
	bool bBuffOtherDefence;

	UPROPERTY()
	bool bBuffOtherAtk;

	UPROPERTY()
	bool bIsLow;

	UPROPERTY()
	bool bFreezedUp;

	UPROPERTY()
	bool bAttacked;

	UPROPERTY()
	int Bt_Id;
	
	UPROPERTY()
	bool bFlee;

	UPROPERTY()
	bool bRethink = false;
	
	UPROPERTY()
	FString CurrentDecision;
	
	UPROPERTY()
	FString CurrentDecisionTable;
	
	UPROPERTY()
	FString EmotionName;
	
	//------

	static bool bStopTree;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual int GetSpeed() const override;

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
	 * Battle Data simple data asset used to edit buff and debuff increment values 
	 * 
	 * @return Battle Data
	 */
	UBattleData* GetBattleData() const;

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
	
	FString GetEmotionName() const;
	void SetEmotionName(const FString& NewName);

	/**
	 * Highlights the silhouette during AI turn
	 */
	void HighlightsSilhouette();
	/**
	 * Disable the Silhouette
	 */
	void DisableSilhouette();

	void SetIsRespawned(const bool& Value);
	bool IsRespawned() const;
	void ReinizializeTree();

	void StartDefenceMinigame();
	virtual void Heal(const float& RestoredHealth);
	
	void InflictDamage();

	void SetIsBusy(const bool& Value);
	bool IsBusy() const;

	void PlayTurn();

	void SetIsReadyToPlay(const bool& Value);
	bool IsAIReadyToPlay() const;

	bool IsAlive();
	
	void EndTurn();
	static bool MinigameEnded;
	
	UPROPERTY() FDamage RefDamage;

	FEmotionStat& GetStats();

	EMobType GetMobType() const;


	/**
	 * Check if the minigame has started
	 * @return true if started
	 */
	static bool IsMinigameStarted();

	bool IsMinigameHasStarted() const;

	bool IsMinigameEnded() const;
	void SetMinigameEnd(const bool& Value);

	/**
	 * Set the minigame bool value
	 * @note this function is used for behavior decorator attack and retreat
	 * @param Value true or false
	 */
	static void SetMinigameStarted(const bool& Value);

	void SetMinigameHasStarted(const bool& Value);

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
	
	FString GetCurrentDecision() const;
	void SetCurrentDecision(const FString& InCurrentDecision);
	FString GetCurrentDecisionTable() const;
	void SetCurrentDecisionTable(const FString& InCurrentDecisionTable);


	bool IsEAnger() const;
	
	bool IsEFear() const;
	
	bool IsEDisgust() const;

	bool IsESadness() const;

	bool IsEAnxiety() const;

	bool IsECalm() const;

	bool IsEJealousy() const;

	bool IsEShame() const;
	
	/**
	 * Check if current Emotion can heal or not
	 * @return true if healer
	 */
	bool IsHealer() const;

	void SetIsBuffedAtk(const bool& Value);
	
	void SetIsTargetDefDebuffed(const bool& Value);

	void SetIsAshamedState(const bool& Value);
	
	void SetIsLow(const bool& Value);
	
	void SetIsFreezedUp(const bool& Value);

	void SetIsAttacked(const bool& Value);

	void SetIsEnvyBurned(const bool& Value);

	void SetTreeId(const int& Value);

	void SetDebuffShield(const bool& Value);

	void SetDebuffOtherShield(const bool& Value);

	void SetHeal(const bool& Value);

	void SetHealOther(const bool& Value);

	void SetBuffedDefence(const bool& Value);
	
	void SetBuffOtherDefence(const bool& Value);

	void SetBuffOtherAtk(const bool& Value);

	void SetPlayerDebuffAttack(const bool& Value);

	bool GetIsIsBuffedAtk() const;
	bool GetIsTargetDefenceDebuffed() const;
	bool GetIsIsAshamedState() const;
	bool GetIsIsLow() const;
	bool GetIsIsFreezedUp() const;
	bool GetIsIsAttacked() const;
	bool GetIsIsEnvyBurned() const;
	bool GetIsDebuffShield() const;
	bool GetIsDebuffOtherShield() const;
	bool GetIsHeal() const;
	bool GetIsHealOther() const;
	bool GetIsBuffedDefence() const;
	bool GetBuffOtherAtk() const;
	bool GetPlayerDebuffAttack() const;
	bool GetBuffOtherDefence() const;
	int GetTreeId() const;
	bool GetRethink() const;
	void SetRethink(const bool& Value);
	
	bool IsFleeing() const;
	void SetFleeing(const bool& Value);
	
	void Flee();
	
	UFUNCTION() void UpdateBestiaryUI();

	// DEBUG BESTIARY
	UFUNCTION(BlueprintCallable, Category = "Bestiary")
	FString GetNoteKeyForMobType() const;
	UFUNCTION(BlueprintCallable, Category = "Bestiary")
	EEmotionType GetEmotionForMobType() const;
	UFUNCTION(BlueprintCallable, Category = "Bestiary")
	void UnlockContentOnDeath();

	UDecisionTable* GetDecisionTable() const;

	/**
	 * If the health is 20% of the max health, health will be considered low
	 * @return true if low health
	 */
	bool IsLowHealth() const;
	
	UIccEmotionAnimDealer* GetAnimationDealer();

private:

	UPROPERTY()
	AMinigameHandler* MinigameHandler = nullptr;

	UPROPERTY()
	bool bEnableSilhouette = false;

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

	UPROPERTY()
	bool bMinigameStarted = false;

	UPROPERTY()
	bool bMinigameEnded = false;

	/**
	 * Is ready to play turn or not
	 */
	UPROPERTY()
	bool bIsReady = false;

	UPROPERTY()
	bool bRespawned = false;

	static UICantCryGameInstance* GameRef;

	UPROPERTY()
	bool bHasUnlockedContent = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Behaviors", meta=(AllowPrivateAccess=true))
	UDecisionTable* DecisionTable;
};
