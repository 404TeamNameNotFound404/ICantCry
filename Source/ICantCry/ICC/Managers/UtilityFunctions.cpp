

#include "UtilityFunctions.h"

#include "Kismet/GameplayStatics.h"

void UtilityFunctions::LoadSceneByName(UWorld* World, const FName& Name)
{
	UGameplayStatics::OpenLevel(World, Name);
}

void UtilityFunctions::LoadBattleSceneRandom(UWorld* World, const TArray<FName>& BattleSceneRandomNames)
{
	if (!World) return;
	
	const int Aleatory = FMath::RandRange(0, BattleSceneRandomNames.Num() - 1);
	
	UGameplayStatics::OpenLevel(World, BattleSceneRandomNames[Aleatory]);
}
