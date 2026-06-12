// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NarrativeTriggerBox.generated.h"


class UBoxComponent;
class UDialogueAsset;
class AICC_Player;
class AICC_PlayerController;
class UBarkWidget;

UCLASS()
class ICANTCRY_API ANarrativeTriggerBox : public AActor
{
    GENERATED_BODY()
	
public:	
	
	ANarrativeTriggerBox();

protected:
	
	virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UBoxComponent> TriggerZone;

    /** The dialogue that should start when the player enters the trigger */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
    TObjectPtr<UDialogueAsset> DialogueToTrigger;

    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Narrative")
    bool bTriggerOnce = true;

   
    UPROPERTY(EditAnywhere, Category = "Narrative")
    TSubclassOf<class UBarkWidget> BarkWidgetClass;

private:

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    bool bHasBeenTriggered = false;
    bool bPlayerInside = false;

};
