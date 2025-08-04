

#include "UtilityFunctions.h"

#include "Kismet/GameplayStatics.h"

void UtilityFunctions::LoadSceneByName(UWorld* World, const FName& Name)
{
	UGameplayStatics::OpenLevel(World, Name);
}
