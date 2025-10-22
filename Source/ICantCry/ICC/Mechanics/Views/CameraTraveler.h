#pragma once
#include "CoreMinimal.h"
#include "PlayerTeleportObj.h"
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



private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* BoxComponents;

	UPROPERTY()
	bool bPlayerOverlapped = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	AICC_Player* Player;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	TSoftObjectPtr<APlayerTeleportObj> DesiredTeleportLocation;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Traveler", meta = (AllowPrivateAccess = "true"))
	bool bPlayerMustTeleport = false;
};
