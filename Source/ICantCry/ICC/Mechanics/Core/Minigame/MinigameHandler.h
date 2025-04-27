#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinigameHandler.generated.h"

/**
 * Class must be placed inside battle scene to handle the minigame behavior
 * Basic knowledge: It will contain both attack / defence blueprints, and it will
 * load it at runtime when needed.
 */

class AICC_Player;

UCLASS(Blueprintable)
class ICANTCRY_API AMinigameHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinigameHandler();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	/**
	 * Instantiate the minigame and add it to the viewport.
	 * Enable the ui input automatically
	 * When the player attack the minigame will always show the attack minigame (true by default)
	 * When AI will attack will always be false so it must be set in the AI move to false.
	 * For the player it must be connected once the Attack button is pressed
	 * For the enemy once ai start to attack
	 * @param EnableAttack by default is set to true which means that it will displays the Attack minigame
	 */
	UFUNCTION(BlueprintCallable)
	void StartMinigame(const bool& EnableAttack = true);

	/**
	 * Close the minigame once it's finished. Must be called at the end of each minigame
	 * so or via blueprint or via c++ so by default I've set it as UFUNCTION blueprint callable
	 */
	UFUNCTION(BlueprintCallable)
	void EndMinigame();

	/**
	 * Get The Combat player
	 * @note this probably will be removed
	 * @return Player
	 */
	AICC_Player* GetBattlePlayer() const;


private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> AttackMinigame;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minigame", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> DefenseMinigame;

	/**
	 * Display the current minigame displayed
	 */
	UPROPERTY()
	UUserWidget* CurrentMinigameDisplayed;

	UPROPERTY()
	AICC_Player* Player = nullptr;
};
