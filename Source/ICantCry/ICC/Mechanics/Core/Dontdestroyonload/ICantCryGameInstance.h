#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "ICantCry/ICC/Actors/Player/ICC_PlayerMemory.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PersistentData.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleFlow/DamageCalculator.h"
#include "ICantCry/ICC/Inventory/Inventory.h"
#include "ICantCry/ICC/Actors/EmotionsData/EmotionsData.h"
#include "ICantCry/ICC/Actors/NoteData/NoteData.h"
#include "GameplayTagContainer.h"
#include "ICantCry/ICC/Mechanics/TurnSystem/BattleMemory/FBattleMemory.h"
#include "ICantCryGameInstance.generated.h"

class AICC_Player;
class UBestiaryUI;

/**
 * DontDestroyOnLoad like class
 * @note class should be added in project settings -> instance -> add this 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UICantCryGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual void Shutdown() override;

	void RecreatePlayer() const;
	/**
	 * Function is deprecated please use StoreBeginPlayerTransform
	 * @param BeginPosition 
	 * @param BeginOrientation 
	 */
	[[deprecated]] void StoreBeginPlayerTransform(const FVector& BeginPosition, const FRotator& BeginOrientation) const;
	void StoreBeginPlayerTransform(AICC_Player* Player, const FVector BeginPosition, const FRotator& BeginOrientation);
	void StoreLastPlayerTransform(AICC_Player* Player, const FVector& LastPosition, const FRotator& LastOrientation);
	void StoreLastPlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const;
	void SavePlayerTransformBegin(AICC_Player* Player, const bool& PreFight = false);
	[[deprecated]] void SavePlayerTransform(const FVector& LastPosition, const FRotator& LastOrientation) const;

	UPlayerStats* GetPlayerStats() const;
	UPersistentData* GetPersistentData() const;
	FDamage* GetCurrentDamageData();
	void SetPlayerStats(UPlayerStats* Stats);
	void SetPersistentPlayer(AICC_Player* Player);
	AICC_Player* GetCurrentPlayer() const;
	void SetDamageData(const FDamage* Damage);

	void SetInventory(const FInventory& Inv);
	FInventory& GetInventory();

	bool CanRecreatePlayer() const;
	void SetCanRecreatePlayer(const bool& Value);
	FPlayerMemory& GetPlayerRuntimeData();

	FRuntimeStats& GetRuntimeStats();

	UPROPERTY()
	bool bHealthInitialized = false;
	
	// Bestiary
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bestiary")
    TArray<FString> CollectedNotes;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bestiary")  
    TSet<EEmotionType> UnlockedEmotions;

	//For Narrative Spawn
	void SetLastMainMapName(FName MapName); 

	UFUNCTION(BlueprintCallable, Category = "Battle")
    FName GetLastMainMapName() const;

	/**
     * Reference to the active Bestiary UI
     * Set this from Blueprint when the Bestiary is opened
     */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Bestiary")
    UBestiaryUI* ActiveBestiaryUI;

    /**
     * Update Bestiary UI if it's currently active
     */
    UFUNCTION(BlueprintCallable, Category = "Bestiary")
    void UpdateBestiaryUI();

    /**
     * Check if Bestiary UI is currently active
     */
    UFUNCTION(BlueprintCallable, Category = "Bestiary")
    bool IsBestiaryUIActive() const;

	//[DEBUG]
	UFUNCTION(BlueprintCallable, Category = "Debug|Bestiary")
	void DebugUnlockAllEmotionsAndNotes();

	UFUNCTION(BlueprintCallable, Category = "Debug|Bestiary")
	void DebugResetBestiary();

	UFUNCTION(BlueprintCallable, Category = "Debug|Bestiary")
	void DebugUnlockSpecificMobContent(FString MobType);
	//[DEBUG END]
	
	UDataTable* GetRecipes() const;
	UDataTable* GetCasingsTable() const;
	UDataTable* GetEssencesTable() const;
	TMap<FString, UTexture2D*>& GetIconMap();
	
	
	// --- LOGICA INVENTARIO TAG (per consegnare oggetti a npc o per eventi) ---
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void AddToInventory(FGameplayTag ItemTag, int32 Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool RemoveFromInventory(FGameplayTag ItemTag, int32 Amount);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    int32 GetItemCount(FGameplayTag ItemTag) const;


	UPROPERTY() FName LastBattleSceneLoaded;
	UPROPERTY() FBattleMemory CachedBattleMemory;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPersistentData* PersistentData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	UPlayerStats* PlayerStats;

	UPROPERTY()
	FRuntimeStats RuntimeStats;
	
	UPROPERTY()
	FCameraMemory CameraMemory;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FDamage StoredDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	FInventory Inventory;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="DontDestroyOnLoad", meta=(AllowPrivateAccess=true))
	TSubclassOf<AICC_Player> PlayerBp;
	
	UWorld* TryGetWorld() const;

	UPROPERTY()
	AICC_Player* PersistentPlayer = nullptr;

	UPROPERTY()
	AICC_Player* CurrentPlayer = nullptr;

	UPROPERTY()
	FPlayerMemory PlayerRuntimeData;

	UPROPERTY()
	bool bCanRecreatePlayer = false;

	// Variable to save the name of the source map
	UPROPERTY()
	FName LastMainMapName;


	// Il "magazzino" degli oggetti fisici
    UPROPERTY()
    TMap<FGameplayTag, int32> InventoryShelf;


	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data Tables", meta=(AllowPrivateAccess=true))
	UDataTable* Recipes;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data Tables", meta=(AllowPrivateAccess=true))
	UDataTable* CasingsTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data Tables", meta=(AllowPrivateAccess=true))
	UDataTable* EssencesTable;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="ICCMisc", meta=(AllowPrivateAccess=true))
	TMap<FString, UTexture2D*> IconPack;
};


