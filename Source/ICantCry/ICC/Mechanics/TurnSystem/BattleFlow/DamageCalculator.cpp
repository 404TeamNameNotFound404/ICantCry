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
		
		const int AIDamageResult = ((((AIMoves->MovePower / (2 - AIMoves->MinigamePower)) * (EnemyData->AttackPower / PlayerStats->DefencePower))) * AIMoves->ActionPointsModifier * AIMoves->WeaknessModifier) * PlayerStats->MinigameModifier;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(AIDamageResult));
		return AIDamageResult;
	}
	
	return 0;
}

