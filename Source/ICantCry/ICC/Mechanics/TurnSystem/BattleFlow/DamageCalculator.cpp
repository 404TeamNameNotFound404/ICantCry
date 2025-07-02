#include "DamageCalculator.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

UDamageCalculator::UDamageCalculator()
{
}


UDamageCalculator::UDamageCalculator(UBulletData* Data, UPlayerStats* Stats , UEnemyDatas* EnemyData ,UEnemyTactics* Moves) 
{
	DamageMath.BulletData = Data;
	DamageMath.PlayerStats = Stats;
	DamageMath.AIMoves = Moves;
	DamageMath.EnemyData = EnemyData;
}


int FDamage::CalculateDamage(const bool& IsPlayerAttacking)
{
	if (!PlayerStats || !EnemyData || !BulletData || !AIMoves) //  BulletData is null 
	{
		DebugHelper::LogError("PlayerStats is null in FDamage::CalculateDamage, can't do math");
		return 0;
	}
	
	if (IsPlayerAttacking)
	{
		DebugHelper::LogSuccess("Calculating damage Player -> Enemy");
		DebugHelper::LogMessage(10, FColor::Purple, "Power " + FString::FromInt(BulletData->Power) + "\nMinigameModifier " + FString::FromInt(PlayerStats->MinigameModifier) + "\nAttack " + FString::FromInt(PlayerStats->AttackPower) +
			"\nDefencePower " + FString::FromInt(PlayerStats->DefencePower) + "\nAP " + FString::FromInt(PlayerStats->ApModifier) + "\nWeakness " + FString::SanitizeFloat(PlayerStats->WeaknessModifier));
		const int Result = (((BulletData->Power / (2 - PlayerStats->MinigameModifier)) * (PlayerStats->AttackPower / PlayerStats->DefencePower))) * PlayerStats->ApModifier * PlayerStats->WeaknessModifier;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(Result));
		return Result;
	}

	if (!IsPlayerAttacking)
	{
		DebugHelper::LogSuccess("Calculating damage Enemy -> Player");
		const int AIDamageResult = (((BulletData->Power / (2 - AIMoves->MinigamePower)) * (EnemyData->AttackPower / EnemyData->DefencePower))) * AIMoves->ActionPointsModifier * AIMoves->WeaknessModifier;
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::FromInt(AIDamageResult));
		return AIDamageResult;
	}
	
	return 0;
}

