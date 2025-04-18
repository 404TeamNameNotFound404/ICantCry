#include "DamageCalculator.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"


FDamage::FDamage() : BulletData(nullptr), PlayerStats(nullptr), AIMoves(nullptr), EnemyData(nullptr)
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
	if (IsPlayerAttacking)
	{

		if (!PlayerStats || !EnemyData || !BulletData || !AIMoves)
		{
			DebugHelper::LogError("PlayerStats is null in FDamage::CalculateDamage, can't do math");
			return 0;
		}
		
		DebugHelper::LogSuccess("Calculating damage Player -> Enemy");
		const int Result = (((BulletData->Power / (2 - PlayerStats->MinigameModifier)) * (PlayerStats->AttackPower / EnemyData->DefencePower))) * PlayerStats->ApModifier * BulletData->WeaknessModifier;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(Result));
		return Result;
	}

	if (!IsPlayerAttacking)
	{
		if (!PlayerStats || !EnemyData || !AIMoves)
		{
			DebugHelper::LogError("Player stats or EData or AIMoves are null!");
			return 0;
		}
		
		DebugHelper::LogSuccess("Calculating damage Enemy -> Player");
		DebugHelper::LogWarning("Move Power -> " + FString::FromInt(AIMoves->MovePower));
		DebugHelper::LogWarning("AI Minigame power -> " + FString::FromInt(AIMoves->MinigamePower));
		DebugHelper::LogWarning("Enemy Attack Power  -> " + FString::FromInt(EnemyData->AttackPower));
		DebugHelper::LogWarning("Player Defence Power  -> " + FString::FromInt(PlayerStats->DefencePower));
		DebugHelper::LogWarning("Action point modifier  -> " + FString::FromInt(AIMoves->ActionPointsModifier));
		DebugHelper::LogWarning("Action weakness modifier  -> " + FString::FromInt(AIMoves->WeaknessModifier));
		DebugHelper::LogWarning("Player minigame modifier  -> " + FString::FromInt(PlayerStats->MinigameModifier));
		const int AIDamageResult = (((AIMoves->MovePower / (2 - AIMoves->MinigamePower)) * (EnemyData->AttackPower / PlayerStats->DefencePower))) * AIMoves->ActionPointsModifier * AIMoves->WeaknessModifier; // Missing PlayerStats->MinigameModifier?
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(AIDamageResult));
		return AIDamageResult;
	}
	
	return 0;
}

