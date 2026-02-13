#include "DamageCalculator.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"

FDamage::FDamage(): BulletData(nullptr), PlayerStats(nullptr), AIMoves(nullptr), EnemyData(nullptr)
{
}

FDamage::FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData, UICantCryGameInstance* GI ) 
{
	BulletData = BData;
	PlayerStats = PStats;
	AIMoves = AITactics;
	EnemyData = EData;
	Instance = GI;
}

int FDamage::CalculateDamage(const bool& IsPlayerAttacking)
{
	// checkf(PlayerStats, TEXT("PlayerStats is null"));
	// checkf(AIMoves, TEXT("AIMoves is null"));
	// checkf(EnemyData, TEXT("E data is null"))
	// checkf(AIMoves, TEXT("Moves null"))

	if (!PlayerStats || !AIMoves || !EnemyData || !AIMoves)
	{
		return 0;
	}
	
	FRuntimeStats& RuntimeStats = Instance->GetRuntimeStats();
	UPlayerStats* Stats = Instance->GetPlayerStats();
	
	if (IsPlayerAttacking)
	{
		float Coefficient = 0.5f; // fallback in case of invalid parameters

		if (BulletData->Coefficients.Contains(EnemyData->Type))
		{
			Coefficient = BulletData->Coefficients[EnemyData->Type];
		}
		
		DebugHelper::LogMessage(10, FColor::Purple, "MinigameModifier on damage math -> " + FString::SanitizeFloat(Stats->MinigameModifier));
		
		const float Result = (((BulletData->Power / (2 - RuntimeStats.MinigameModifier)) * (RuntimeStats.AttackPower / EnemyData->DefencePower))) * (RuntimeStats.ApModifier * Coefficient/*BulletData->WeaknessModifier*/);
		const int RoundedResult = FMath::RoundToInt(Result);

		DebugHelper::AddMessageToLog("[DamageMath]: Data gathered for dmg math:\nBullet Pwr : " + FString::SanitizeFloat(BulletData->Power) +"\nMinigame Modifier " + FString::SanitizeFloat(RuntimeStats.MinigameModifier)
			+ "\nPlayer Atk: " + FString::SanitizeFloat(RuntimeStats.AttackPower) + "\nTarget Def : " + FString::SanitizeFloat(EnemyData->DefencePower) +
			"\nPlayer Ap Modifier " + FString::SanitizeFloat(RuntimeStats.ApModifier) + "\nWeaknessModifier " + FString::SanitizeFloat(Coefficient));
		
		DebugHelper::AddMessageToLog("[DamageMath]: Formula is -> ( " +FString::SanitizeFloat(BulletData->Power)  + ") / (2 - " + FString::SanitizeFloat(RuntimeStats.MinigameModifier) +
			") x (" + FString::SanitizeFloat(RuntimeStats.AttackPower) + " / " + FString::SanitizeFloat(EnemyData->DefencePower) + ") x (" + FString::SanitizeFloat(RuntimeStats.ApModifier) + " x " + FString::SanitizeFloat(Coefficient) + ")");

		DebugHelper::AddMessageToLog("[DamageMath]: Risultato approssimato per eccesso (Ceil Result) -> " + FString::SanitizeFloat(RoundedResult));
		
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::SanitizeFloat(RoundedResult));
		DebugHelper::AddMessageToLog("[DamageMath]: Damage dealt -> " + FString::SanitizeFloat(RoundedResult));
		return RoundedResult;
	}

	if (!IsPlayerAttacking)
	{
		
		if (!PlayerStats || !EnemyData || !AIMoves)
		{
			DebugHelper::LogError("Player stats or EData or AIMoves are null!");
			return 0;
		}
		
		const float AIDamageResult = ((((AIMoves->MovePower / (2 - AIMoves->MinigamePower)) * (EnemyData->AttackPower / RuntimeStats.DefencePower))) * AIMoves->ActionPointsModifier * AIMoves->WeaknessModifier) * PlayerStats->MinigameModifier;
		const int RoundedResult = FMath::RoundToInt(AIDamageResult);
		
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::SanitizeFloat(RoundedResult));

		DebugHelper::AddMessageToLog("[DamageMath]: AI To Player Formula is -> ( " +FString::SanitizeFloat(AIMoves->MovePower)  + ") / (2 - " + FString::SanitizeFloat(AIMoves->MinigamePower) +
			") x (" + FString::SanitizeFloat(EnemyData->AttackPower) + " / " + FString::SanitizeFloat(RuntimeStats.DefencePower) + ") x (" + FString::SanitizeFloat(AIMoves->ActionPointsModifier) + " x " +
			" * " + FString::SanitizeFloat(RuntimeStats.MinigameModifier) + ")");

		DebugHelper::AddMessageToLog("[DamageMath]: Damage dealt -> " + FString::SanitizeFloat(AIDamageResult));
		return RoundedResult;
	}
	
	return 0;
}

