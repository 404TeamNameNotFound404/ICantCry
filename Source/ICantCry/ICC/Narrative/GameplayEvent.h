// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayEvent.generated.h"


class AICC_Player;

/**
 * CLASSE: UGameplayEvent
 * DESCRIZIONE: Classe base astratta per ogni azione narrativa.
 * Consente ai designer di creare eventi personalizzati (es. "Apri Porta", "Avvia Cutscene")
 * che possono essere inseriti dentro Quest o Dialoghi come Data Assets.
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class ICANTCRY_API UGameplayEvent : public UObject
{
	GENERATED_BODY()

	public:
	/**
	 * Funzione che esegue la logica dell'evento.
	 * @param Player: Il giocatore che scatena o riceve l'evento.
	 * @param Context: L'oggetto che ha causato l'evento (es. l'NPC o un Trigger).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Narrative")
	void ExecuteEvent(AICC_Player* Player, UObject* Context);

	virtual void ExecuteEvent_Implementation(AICC_Player* Player, UObject* Context);
	
};
