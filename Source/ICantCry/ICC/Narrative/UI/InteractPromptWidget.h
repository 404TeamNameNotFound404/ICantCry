// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InputCoreTypes.h"
#include "InteractPromptWidget.generated.h"

class UTextBlock;
class UImage;
class UTexture2D;

/**
* C++ CLASS: UInteractPromptWidget
* Handles the close-up prompt by displaying the button's action and dynamic icon.
*/
UCLASS()
class ICANTCRY_API UInteractPromptWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    /** Function called from C++ to set the action text and pass the button to the Blueprint */
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    void SetPromptData(FKey BoundKey, const FText& ActionText);

protected:

    virtual void NativeConstruct() override;
    
    UPROPERTY(meta = (BindWidget)) TObjectPtr<UTextBlock> ActionTextBlock;

    UPROPERTY(meta = (BindWidget)) TObjectPtr<UImage> KeyImage;

    bool bIsFocusable;
    
    /** A fantastic map configurable from the Editor!
    * Associates a physical button (Key) with a 2D Texture (Icon).
    */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|UI")
    TMap<FKey, UTexture2D*> KeyIconMap;

    /** Trapable event in Blueprint to change KeyImage texture based on key press */
    UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
    void OnKeyPromptUpdated(FKey BoundKey);
};
