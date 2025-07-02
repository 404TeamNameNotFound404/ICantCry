// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusTracker.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ICANTCRY_API UStatusTracker : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusTracker();

protected:
	/**
	 * Used to store if the owner of this component is afflicted by
	 * status
	 */
	UPROPERTY()
	bool bIsOwnerAfflicted;
	
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	/*-------------------- AI CHECKS --------------------*/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/
	
	/**
	 * Check if there's status applied to the current actor and if so return true.
	 * To be defined
	 * @return true if status is applied
	 */
	bool IsAfflicted();

	/**
	 * Used to set the afflicted status,
	 * - for the AI must be set
	 *    through player bullet / shoot
	 * - for the player it must be set inside the AI
	 *    behavior tree
	 * @param Applied 
	 */
	void SetStatusApplied(const bool& Applied);

	
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/
	/*-------------------- AI CHECKS --------------------*/


	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/


	

	/*-------------------- PLAYER CHECKS --------------------**/
	/*----------DO NOT WRITE ANYTHING IN THIS SPACE -------------*/
	
};
