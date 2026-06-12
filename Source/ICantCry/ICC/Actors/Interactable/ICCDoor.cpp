// Fill out your copyright notice in the Description page of Project Settings.


#include "ICCDoor.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"


#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Mechanics/Core/Dontdestroyonload/ICantCryGameInstance.h"


// Sets default values
AICCDoor::AICCDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AICCDoor::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AICCDoor::OnOverlapEnd);

}

// Called when the game starts or when spawned
void AICCDoor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AICCDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AICCDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult) 

{
	AICC_Player* Player = Cast<AICC_Player>(OtherActor);
	if (!Player) return;

	
	if (bPlayerInside) return;
	bPlayerInside = true;

	// door locked
	if (bIsLocked)
	{
		UICantCryGameInstance* GI = Cast<UICantCryGameInstance>(Player->GetGameInstance());
		const bool bHasKey = GI && KeyItemTag.IsValid() && GI->GetItemCount(KeyItemTag) > 0;

		if (!bHasKey)
		{
			Player->PlayBarkImmediately(LockedBark);
			return; // no key: comment and DO NOT enter (bPlayerInside remains true)
		}

		if (GI)
		{
			GI->RemoveFromInventory(KeyItemTag, 1);
		}

		if (UnlockSound)
		{
			UGameplayStatics::PlaySound2D(GetWorld(), UnlockSound);
		}
	

		if (UnlockAudioComponent && UnlockAudioComponent->IsPlaying())
		{
			UnlockAudioComponent->Stop();
			UnlockAudioComponent = nullptr; 
		}
	}

	//enter ( fade in)
	ULevelSequencePlayer* LevelSequencePlayer =
		ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), FadeIn, FMovieSceneSequencePlaybackSettings(), SeqActor);

	if (LevelSequencePlayer)
	{
		LevelSequencePlayer->Play();
	}
}


void AICCDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<AICC_Player>(OtherActor) && !BoxComponent->IsOverlappingActor(OtherActor))
	{
		bPlayerInside = false;
	}
}

