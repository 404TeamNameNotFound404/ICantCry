// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "ICantCry/ICC/Narrative/Core/QuestManagerSystem.h"
#include "QuestEntryWidget.generated.h"


class UCharacterUI;

/**
 * Represents a single row in the ScrollBox's mission list.
 */
UCLASS()
class ICANTCRY_API UQuestEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** called by the Character UI right after creating the row, stores the quest and shows its title */
	void SetupQuestEntry(const FQuestProgress& InProgress, class UCharacterUI* InParentUI);

	/** placeholder, currently not implemented */
	void UpdateObjectiveDisplay(const FQuestProgress& Details);

protected:
	virtual void NativeConstruct() override;

	/** button covering the whole row, click opens/closes the quest details */
	UPROPERTY(meta = (BindWidget))
	UButton* BtnSelect;

	/** text showing the quest title in the list */
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextQuestTitle;

	/** snapshot of the quest at the moment the row was built, used only to know which quest this row is */
	FQuestProgress StoredProgress;

	/** Character UI that owns this row, receives the click */
	UPROPERTY()
	TObjectPtr<class UCharacterUI> ParentUI;

	/** click handler of BtnSelect, forwards the quest id to the Character UI toggle */
	UFUNCTION()
	void OnRowClicked();
};
