#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Actors/ICC_Actor.h"
#include "BattleVisualizer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ICANTCRY_API UBattleVisualizer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* MainText;
	
	
};
