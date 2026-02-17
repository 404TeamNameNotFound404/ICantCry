// Fill out your copyright notice in the Description page of Project Settings.
#include "DebugHelper.h"
#include "EngineUtils.h"

TArray<FString> DebugHelper::LogMessages;

void DebugHelper::LogSuccess(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Green, Message);
}

void DebugHelper::LogWarning(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Yellow, Message);
}

void DebugHelper::LogError(const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Red, Message);
}

void DebugHelper::LogMessage(const float& TimeToDisplay, const FColor& Color, const FString& Message)
{
	if (!GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, Message);
}


AActor* DebugHelper::FindActorWithTag(UWorld* World, const FName& Tag)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->ActorHasTag(Tag))
		{
			return *It;
		}
		break;
	}

	return nullptr;
}

AActor* DebugHelper::FindActorByName(UWorld* World, const FName& Name)
{
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (It->GetName() == Name)
		{
			return *It;
		}
		
		break;
	}

	return nullptr;
}

void DebugHelper::AddOverlayMaterialToStaticMesh(UStaticMeshComponent* Target)
{
	if (!Target)
	{
		return;
	}

	UMaterialInterface* OverlayMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/ICC/BluePrints/Actors/Materials/OutLiner/M_Outliner.M_Outliner'"));
	checkf(OverlayMaterial, TEXT("Could not find Material 'Outliner'"));
	UMaterialInstanceDynamic* OverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, Target);
	Target->OverlayMaterial = OverlayMaterialInstance;
	Target->MarkRenderStateDirty();
}

void DebugHelper::AddOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target, const FString& MaterialName)
{
	if (!Target)
	{
		LogError("Target is null at DebugHelper::AddTurnMaterialOverlayToSkeletalMesh");
		return;
	}

	const FString MaterialPath = TEXT("Material'/Game/ICC/BluePrints/Actors/Materials/OutLiner/") + MaterialName + TEXT(".") + MaterialName;
	UMaterialInterface* OverlayMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/ICC/BluePrints/Actors/Materials/OutLiner/M_TurnOutliner.M_TurnOutliner'"));
	checkf(OverlayMaterial, TEXT("Could not find Material 'M_TurnOutliner'"));
	
	UMaterialInstanceDynamic* OverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, Target);
	Target->SetMaterial(0, OverlayMaterialInstance); // Apply to the first material slot (index 0)
	Target->MarkRenderStateDirty();
}

void DebugHelper::AddTurnOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target)
{
	if (!Target)
	{
		return;
	}

	UMaterialInterface* OverlayMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/ICC/BluePrints/Actors/Materials/OutLiner/M_TurnOutliner.M_TurnOutliner'"));
	checkf(OverlayMaterial, TEXT("Could not find Material 'M_TUrnOutliner'"));

	UMaterialInstanceDynamic* OverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, Target);
	Target->OverlayMaterial = OverlayMaterialInstance;
	Target->MarkRenderStateDirty();
}

void DebugHelper::RemoveTurnOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target)
{
	if (!Target)
	{
		LogError("Target is null at DebugHelper::RemoveOverlayMaterialFromStaticMesh");
		return;
	}

	Target->OverlayMaterial = nullptr;
	Target->MarkRenderStateDirty();
}

void DebugHelper::RemoveOverlayMaterialToASkeletalMesh(USkeletalMeshComponent* Target)
{
	if (!Target)
	{
		LogError("Target is null at DebugHelper::RemoveOverlayMaterialFromStaticMesh");
		return;
	}

	Target->OverlayMaterial = nullptr;
	Target->MarkRenderStateDirty();
}

void DebugHelper::RemoveOverlayMaterialFromStaticMesh(UStaticMeshComponent* Target)
{
	if (!Target)
	{
		LogError("Target is null at DebugHelper::RemoveOverlayMaterialFromStaticMesh");
		return;
	}

	Target->OverlayMaterial = nullptr;
	Target->MarkRenderStateDirty();
}

void DebugHelper::AddTurnMaterialOverlayToStaticMesh(UStaticMeshComponent* Target)
{
	if (!Target)
	{
		return;
	}

	UMaterialInterface* OverlayMaterial = LoadObject<UMaterialInterface>(nullptr, TEXT("Material'/Game/ICC/BluePrints/Actors/Materials/OutLiner/M_TurnOutliner.M_TurnOutliner'"));
	checkf(OverlayMaterial, TEXT("Could not find Material 'M_TUrnOutliner'"));

	UMaterialInstanceDynamic* OverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, Target);
	Target->OverlayMaterial = OverlayMaterialInstance;
	Target->MarkRenderStateDirty();
}

void DebugHelper::RemoveTurnMaterialOverlayToStaticMesh(UStaticMeshComponent* Target)
{
	if (!Target)
	{
		LogError("Target is null at DebugHelper::RemoveOverlayMaterialFromStaticMesh");
		return;
	}

	Target->OverlayMaterial = nullptr;
	Target->MarkRenderStateDirty();
}

void DebugHelper::AddMessageToLog(const FString& Message)
{
	if (LogMessages.Num() > 0 && LogMessages.Last() == Message)
	{
		return;
	}
	
	LogMessages.Add(Message);
}

void DebugHelper::ClearAllLogs()
{
	LogMessages.Empty();
}

void DebugHelper::SaveLogToFile()
{
	const FString FilePath = FPaths::ProjectSavedDir() + TEXT("Logs/");
	const FString TimeStamp = FDateTime::Now().ToString(TEXT("%d%m%Y_%H%M%S"));
	const FString FileName = FString::Printf(TEXT("BattleLog_%s.log"), *TimeStamp);
	const FString FullPath = FilePath + FileName;
	const FString CombinedLog = FString::Join(LogMessages, TEXT("\n"));
	FFileHelper::SaveStringToFile(CombinedLog, *FullPath);
}

bool DebugHelper::IsGamepadPlugged()
{
	if (!FSlateApplication::IsInitialized())
	{
		return false;
	}

	const TSharedPtr<GenericApplication> App = FSlateApplication::Get().GetPlatformApplication();

	return App.IsValid() && App->IsGamepadAttached();
}

