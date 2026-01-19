// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Inventory/Inventory.h"
#include "../Inventory/Recipe.h"
#include "CraftingTable.generated.h"


class AICC_Player;

/**
 * 
 */
UCLASS()
class ICANTCRY_API UCraftingTable : public UObject
{
	GENERATED_BODY()

public:

    void Initialize(FInventory* InInventory, UWorld* World); 
	
	void Initialize(UWorld* World); 

	void CraftBullet(const FBullet& BulletToCraft, ERecipeType BlueprintType, ECasingType CasingType);
	void CraftSelectedBullet(UWorld* World);

	bool CanCraft() const { return IsCraftable; };

	void SetRecipe( const FRecipe& InRecipe); // sets which recipe to evaluate
    
	void CheckResources(); // check if u can craft

 	/**
	 * Scan and check the avaiable resources and returns true if all the requirements are met
	 * to craft 
	 * @return true if requirements are met 
	 */
	bool ScanResources();

	bool ScanResources(UWorld* World); 

	void Craft(); // crafting (consumes resources and adds bullet)
	void Craft(UWorld* World);

	//void SetInventoryReference(FInventory* InventoryRef);

	FInventory GetInventory() const;

	TArray<ERecipeType> GetAvailableRecipes() const;

	TArray<FRecipe> GetAvaiableRecipes();


	//////////// NOTA ///////////

	//PERCHE' FUNZIONI L'AGGIUNTA AUTOMATICA DELLE RECIPE NELLA LISTA, SERVE CREARE QUESTA CLASSE 

// 	void ABP_Pickup_Blueprint::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
// {
//     if (AICC_Player* Player = Cast<AICC_Player>(OtherActor))
//     {
//         if (Player->GetInventory())
//         {
			// Assegna o Base o Gold
//             Player->GetInventory()->AddBlueprint(BlueprintType);
//             Destroy();
//         }
//     }
// }

// QUESTA CLASSE VA DATA ALL'OGGETTO CON CUI IL PLAYER ANDRA' A COLLIDERE E VERRA' RACCOLTO
	

private :

	UPROPERTY()
	FInventory Inventory;

	UPROPERTY()
	FRecipe Recipe;

	UPROPERTY()
	FInventory PlayerInventoryRef; // Puntatore all'inventario del player

	UPROPERTY()
	bool IsCraftable = false;

	// Lista di tutte le ricette esistenti nel gioco
	// uso MasterRecipes e creo AvailableRecipes per la separazione tra ricette bloccate/sbloccate
	
    UPROPERTY()
    TArray<FRecipe> MasterRecipes;

	UPROPERTY()
	AICC_Player* Player;

	
};
