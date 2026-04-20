#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurnBasedSystem.h"
#include "ICantCry/ICC/Input/ICC_PlayerController.h"
#include "ICantCry/ICC/Mechanics/UI/BattleNotifiers/BattleInfo.h"
#include "ICantCry/ICC/Managers/EnemySpawnManager.h"
#include "BattleHandler.generated.h"

UCLASS(Blueprintable)
class ICANTCRY_API ABattleHandler : public AActor
{
	GENERATED_BODY()

public:
	ABattleHandler();


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UTurnBasedSystem* GetTurnBasedSystem() const;

	UBattleInfo* GetBattleInfo() const;

	AEnemySpawnManager* GetEnemySpawnManager();

	bool IsControllerPlugged() const;
	
	void Fire(const FVector& DeltaLocation ,const FLinearColor& Color);
	
	void SimulateHurt(const FLinearColor& Color);
	
	void SimulateAura(AICC_Actor* Target ,const float& SpawnRate ,const FLinearColor& Color);
	void SimulateAura(AICC_Actor* Target, const float& SpawnRate , const FLinearColor& Color, const EBuffStatus& Status);
	void IncreaseAura(const float& Value);
	void DecreaseAura(const float& Value);
	void DeactivateAura(const EBuffStatus& Status);
	void DeactivateAura();
	
	UBulletData* GetIndifferenceData();

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTurnBasedSystem* TurnBasedSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category= "Battle Info" ,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UBattleInfo> BattleInfoWidget ;

	/**
	 * Shoot VFX
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle VFX", meta=(AllowPrivateAccess="true"))
	UNiagaraSystem* MuzzleFlash;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle VFX", meta=(AllowPrivateAccess="true"))
	UNiagaraSystem* HurtPrefab;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle VFX", meta=(AllowPrivateAccess="true"))
	UNiagaraSystem* AuraPrefab;
	
	UPROPERTY() TMap<TEnumAsByte<EBuffStatus>, UNiagaraComponent*> ActiveAuras;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle VFX", meta=(AllowPrivateAccess="true"))
	float AuraDecreaseValue = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Battle VFX", meta=(AllowPrivateAccess="true"))
	float AuraIncreaseValue = 500.f;
	
	// To convert wasted bullet into indifference if not found
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Turn", meta=(AllowPrivateAccess=true))
	UBulletData* Indifference;
	
	UPROPERTY()
	UNiagaraComponent* Flash;
	
	UPROPERTY()
	UNiagaraComponent* Aura;
	
	UPROPERTY()
	FVector BeamPosition;

	UPROPERTY()
	bool bControllerPlugged = false;
	
	UPROPERTY()
	UBattleInfo* BattleInfo;

	UPROPERTY()
	AEnemySpawnManager* SpawnManager;

	UPROPERTY()
	AICC_PlayerController* PC;
	
	UPROPERTY()
	bool bMovingMuzzle;
	
	UPROPERTY()
	UICantCryGameInstance* Instance;
	
	UPROPERTY()
	FVector StartVfxShootLocation;
	
	UPROPERTY() FTimerHandle BeamTimer;
	
	void UpdateMuzzleFlashPosition(const FVector& Location);
};
