// Fill out your copyright notice in the Description page of Project Settings.


#include "ICC_GameMode.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void AICC_GameMode::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
	checkf(GameInstance, TEXT("GameInstance is null"));

	if (GameInstance->CanRecreatePlayer())
	{
		GameInstance->RecreatePlayer();
		DebugHelper::LogWarning("Recreating player");
		GameInstance->SetCanRecreatePlayer(false);
	}

	DebugHelper::LogSuccess("no need to recreate player");
}


AActor* AICC_GameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    FString StartNodeName = UGameplayStatics::ParseOption(OptionsString, TEXT("PlayerStart"));

    if (!StartNodeName.IsEmpty())
    {
        for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
        {
            APlayerStart* PStart = *It;
            // Verifichiamo se il PlayerStart ha il tag corrispondente
            // Nota: In Unreal, il PlayerStartTag è una FString standard, quindi il confronto rimane semplice
            if (PStart && PStart->PlayerStartTag.ToString() == StartNodeName)
            {
                return PStart;
            }
        }
    }
    return Super::ChoosePlayerStart_Implementation(Player);
}
