#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "CameraTraveler.generated.h"

UCLASS()
class ICANTCRY_API ACameraTraveler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACameraTraveler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,  int32 OtherBodyIndex,  
				bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	void SetIsOneShot(const bool& Value);


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponents;

	UPROPERTY()
	bool bPlayerOverlapped = false;

	/**
* @brief  bIsOneShot is used ONLY when the overlap is done of a blind room. So if true the box will be disabled in order to
* allow the player camera to follows the player on the hallways. Once player leaves the area the box will be re-enabled again
* for future access
* @note Set it to false on the CameraHallway.cpp OnOverlapEnd! or it will breaks
*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	bool bIsOneShot = false;

	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	AICC_Player* Player;
};
