#include "DamageCalculator.h"
#include "ICantCry/ICC/Mechanics/Core/Data/PlayerStats.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"

FDamage::FDamage(): BulletData(nullptr), PlayerStats(nullptr), AIMoves(nullptr), EnemyData(nullptr)
{
}

FDamage::FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData, AICC_Actor* SelfPtr ,UICantCryGameInstance* GI ) 
{
	BulletData = BData;
	PlayerStats = PStats;
	AIMoves = AITactics;
	EnemyData = EData;
	Instance = GI;
	Self = SelfPtr;
}

FDamage::FDamage(UBulletData* BData, UPlayerStats* PStats, UEnemyTactics* AITactics, UEnemyDatas* EData ,const FEmotionStat& EStats,
	AICC_Actor* SelfPtr, UICantCryGameInstance* GI)
{
	BulletData = BData;
	PlayerStats = PStats;
	AIMoves = AITactics;
	EnemyData = EData;
	EmotionStats = EStats;
	Instance = GI;
	Self = SelfPtr;
}

int FDamage::CalculateDamage(const bool& IsPlayerAttacking)
{
	if (!PlayerStats || !AIMoves || !EnemyData || !AIMoves)
	{
		return 0;
	}
	
	
	FRuntimeStats& RuntimeStats = Instance->GetRuntimeStats();
	UPlayerStats* Stats = Instance->GetPlayerStats();
	
	if (IsPlayerAttacking)
	{
		if (Instance->GetCurrentPlayer()->GetBattleHUD()->bIsEvFirst)
		{
			DebugHelper::LogMessage(8, FColor::White, "EV bullet detected return");
			return 0;
		}
		
		float Coefficient = 0.5f; // fallback in case of invalid parameters

		if (BulletData->Coefficients.Contains(EnemyData->Type))
		{
			Coefficient = BulletData->Coefficients[EnemyData->Type];
		}
		
		DebugHelper::LogMessage(10, FColor::Purple, "MinigameModifier on damage math -> " + FString::SanitizeFloat(RuntimeStats.MinigameModifier));
		
		const float Result = (((BulletData->Power / (2 - RuntimeStats.MinigameModifier)) * (RuntimeStats.AttackPower / EmotionStats.DefPower))) * (RuntimeStats.ApModifier * Coefficient/*BulletData->WeaknessModifier*/);
		const int RoundedResult = FMath::RoundToInt(Result);
		Cast<AICC_Player>(Self)->GetRuntimeStats().DbgDamageDealt = RoundedResult;
		
		DebugHelper::AddMessageToLog("[DamageMath]: Doing math under requests of " + Cast<AICC_Player>(Self)->GetCharacterName());
		DebugHelper::AddMessageToLog("[DamageMath]: Data gathered for dmg math:\nBullet Pwr : " + FString::SanitizeFloat(BulletData->Power) +"\nMinigame Modifier " + FString::SanitizeFloat(RuntimeStats.MinigameModifier)
			+ "\nPlayer Atk: " + FString::SanitizeFloat(RuntimeStats.AttackPower) + "\nTarget Def : " + FString::SanitizeFloat(EmotionStats.DefPower) +
			"\nPlayer Ap Modifier " + FString::SanitizeFloat(RuntimeStats.ApModifier) + "\nWeaknessModifier " + FString::SanitizeFloat(Coefficient));
		
		DebugHelper::AddMessageToLog("[DamageMath]: Formula is -> ( " +FString::SanitizeFloat(BulletData->Power)  + ") / (2 - " + FString::SanitizeFloat(RuntimeStats.MinigameModifier) +
			") x (" + FString::SanitizeFloat(RuntimeStats.AttackPower) + " / " + FString::SanitizeFloat(EmotionStats.DefPower) + ") x (" + FString::SanitizeFloat(RuntimeStats.ApModifier) + " x " + FString::SanitizeFloat(Coefficient) + ")");

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
		
		AMob* Emotion = Cast<AMob>(Self);
		
		const float AIDamageResult = ((((Emotion->GetTactics()->MovePower / (2 - Emotion->GetTactics()->MinigamePower)) * (Emotion->GetStats().AtkPower / RuntimeStats.DefencePower))) * Emotion->GetTactics()->ActionPointsModifier * Emotion->GetTactics()->WeaknessModifier) * RuntimeStats.MinigameModifier;
		const int RoundedResult = FMath::RoundToInt(AIDamageResult);
		Emotion->GetStats().DbgDamageDealt = RoundedResult;
		
		DebugHelper::LogMessage(3, FColor::FromHex("433878"), "Damage dealt -> " + FString::SanitizeFloat(RoundedResult) 
			+ " by " + Emotion->GetEmotionName());

		DebugHelper::AddMessageToLog("[DamageMath]: Doing math under requests of " + Emotion->GetEmotionName());
		DebugHelper::AddMessageToLog("[DamageMath]: AI To Player Formula is -> ( " +FString::SanitizeFloat(AIMoves->MovePower)  + ") / (2 - " + FString::SanitizeFloat(AIMoves->MinigamePower) +
			") x (" + FString::SanitizeFloat(EmotionStats.AtkPower) + " / " + FString::SanitizeFloat(RuntimeStats.DefencePower) + ") x (" + FString::SanitizeFloat(AIMoves->ActionPointsModifier) + " x " +
			" * " + FString::SanitizeFloat(RuntimeStats.MinigameModifier) + ")");

		DebugHelper::AddMessageToLog("[DamageMath]: Damage dealt -> " + FString::SanitizeFloat(AIDamageResult));
		return RoundedResult;
	}
	
	return 0;
}

