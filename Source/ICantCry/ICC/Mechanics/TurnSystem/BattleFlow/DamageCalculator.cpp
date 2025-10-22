#include "DamageCalculator.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


FDamage::FDamage(): BulletData(nullptr), PlayerStats(nullptr), AIMoves(nullptr), EnemyData(nullptr)
{
}

FDamage::FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData) 
{
	BulletData = BData;
	PlayerStats = PStats;
	AIMoves = AITactics;
	EnemyData = EData;
}

int FDamage::CalculateDamage(const bool& IsPlayerAttacking)
{
	checkf(PlayerStats, TEXT("PlayerStats is null"));
	checkf(AIMoves, TEXT("AIMoves is null"));
	checkf(EnemyData, TEXT("E data is null"))
	checkf(AIMoves, TEXT("Moves null"))
	
	if (IsPlayerAttacking)
	{
		float Coefficient = 0.5f; // fallback in case of invalid parameters

		if (BulletData->Coefficients.Contains(EnemyData->Type))
		{
			Coefficient = BulletData->Coefficients[EnemyData->Type];
			DebugHelper::LogWarning("Coefficient found!");
		}

		DebugHelper::LogWarning("Coefficient is " + FString::FromInt(Coefficient));
		DebugHelper::LogSuccess("Calculating damage Player -> Enemy");
		DebugHelper::LogMessage(10, FColor::Purple, "Power " + FString::FromInt(BulletData->Power) + "\nMinigameModifier " + FString::FromInt(PlayerStats->MinigameModifier) + "\nAttack " + FString::FromInt(PlayerStats->AttackPower) +
			"\nDefencePower " + FString::FromInt(PlayerStats->DefencePower) + "\nAP " + FString::FromInt(PlayerStats->ApModifier) + "\nWeakness " + FString::SanitizeFloat(PlayerStats->WeaknessModifier));
		const int Result = (((BulletData->Power / (2 - PlayerStats->MinigameModifier)) * (PlayerStats->AttackPower / PlayerStats->DefencePower))) * PlayerStats->ApModifier * BulletData->WeaknessModifier * Coefficient;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(Result));
		DebugHelper::AddMessageToLog("Damage dealt -> " + FString::FromInt(Result));
		return Result;
	}

	if (!IsPlayerAttacking)
	{
		if (!PlayerStats || !EnemyData || !AIMoves)
		{
			DebugHelper::LogError("Player stats or EData or AIMoves are null!");
			return 0;
		}
		
		const int AIDamageResult = ((((AIMoves->MovePower / (2 - AIMoves->MinigamePower)) * (EnemyData->AttackPower / PlayerStats->DefencePower))) * AIMoves->ActionPointsModifier * AIMoves->WeaknessModifier) * PlayerStats->MinigameModifier;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(AIDamageResult));
		DebugHelper::AddMessageToLog("Damage dealt -> " + FString::FromInt(AIDamageResult));
		return AIDamageResult;
	}
	
	return 0;
}

