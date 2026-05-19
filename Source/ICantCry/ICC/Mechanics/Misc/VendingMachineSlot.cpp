// Fill out your copyright notice in the Description page of Project Settings.


#include "VendingMachineSlot.h"

#include "ICantCry/ICC/Actors/Pickups/RecipePickup.h"
#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UVendingMachineSlot::NativeConstruct()
{
	Super::NativeConstruct();
	Setup();
}

FText UVendingMachineSlot::UpdateRecipeName()
{
	if (!Recipes[CurrentRecipeIndex].IsValid())
	{
		return FText();
	}
	
	return FText(FText::FromString(Recipes[CurrentRecipeIndex].GetName(Recipes[CurrentRecipeIndex].RequiredBlueprintType)));
}

FText UVendingMachineSlot::UpdateEssenceName()
{
	if (!Essences[CurrentEssenceIndex].IsValid())
	{
		return FText();
	}
	
	return FText(FText::FromString(Essences[CurrentEssenceIndex].GetName(Essences[CurrentEssenceIndex].EssenceType)));
}

FText UVendingMachineSlot::UpdateEssenceQuantity()
{
	if (!Essences[CurrentEssenceIndex].IsValid())
	{
		return FText();
	}
	
	const FEssence& E = Essences[CurrentEssenceIndex];
	
	return FText(FText::FromString(FString::FromInt(E.Quantity)));
}

FText UVendingMachineSlot::UpdateCasingName()
{
	if (!Casings[CurrentCasingIndex].IsValid())
	{
		return FText();
	}
	
	return FText(FText::FromString(Casings[CurrentCasingIndex].GetName()));
}

FText UVendingMachineSlot::UpdateCasingQuantity()
{
	if (!Casings[CurrentCasingIndex].IsValid())
	{
		return FText();
	}
	
	return FText(FText::FromString(FString::FromInt(Casings[CurrentCasingIndex].GetQuantity())));
}


void UVendingMachineSlot::Setup()
{
	Instance = Cast<UICantCryGameInstance>(GetGameInstance());
	
	SetupRecipes();
	SetupEssences();
	SetupCasings();
	
	RecipeSelection->OnClicked.AddDynamic(this, &UVendingMachineSlot::ScrollRecipe);
	EssenceSelection->OnClicked.AddDynamic(this, &UVendingMachineSlot::ScrollEssence);
	RecipeConfirm->OnClicked.AddDynamic(this, &UVendingMachineSlot::AddRecipe);
	EssenceIncreaseQuantity->OnClicked.AddDynamic(this, &UVendingMachineSlot::IncreaseEssence);
	EssenceDecreaseQuantity->OnClicked.AddDynamic(this, &UVendingMachineSlot::DecreaseEssence);
	EssenceConfirm->OnClicked.AddDynamic(this, &UVendingMachineSlot::AddEssence);
	CasingIncreaseQuantity->OnClicked.AddDynamic(this, &UVendingMachineSlot::IncreaseCasingQuantity);
	CasingDecreaseQuantity->OnClicked.AddDynamic(this, &UVendingMachineSlot::DecreaseCasingQuantity);
	CasingConfirm->OnClicked.AddDynamic(this, &UVendingMachineSlot::AddCasing);
	CasingSelection->OnClicked.AddDynamic(this, &UVendingMachineSlot::ScrollCasing);
	
	Refresh();
}

void UVendingMachineSlot::SetupCasings()
{
	if (!CasingTable)
	{
		DebugHelper::LogError("Casings Data table not found");
		return;
	}
	
	Casings.Empty();
	
	for (TArray<FName> RowNames = CasingTable->GetRowNames();
		const FName& RowName : RowNames)
	{
		
		if (const FCasing* RowPtr = CasingTable->FindRow<FCasing>(RowName, TEXT("Loading Recipes")); 
			RowPtr)
		{
			Casings.Add(*RowPtr); 
		}
	}
}

void UVendingMachineSlot::SetupRecipes()
{
	if (!RecipeTable)
	{
		DebugHelper::LogError("Recipes Data table not found");
		return;
	}
	
	Recipes.Empty();
	
	for (TArray<FName> RowNames = RecipeTable->GetRowNames();
		const FName& RowName : RowNames)
	{
		
		if (const FRecipe* RowPtr = RecipeTable->FindRow<FRecipe>(RowName, TEXT("Loading Recipes")); 
			RowPtr)
		{
			Recipes.Add(*RowPtr); 
		}
	}
	
}

void UVendingMachineSlot::SetupEssences()
{
	if (!EssenceTable)
	{
		DebugHelper::LogError("Essences Data table not found");
		return;
	}
	
	Essences.Empty();
	
	for (TArray<FName> RowNames = EssenceTable->GetRowNames();
		const FName& RowName : RowNames)
	{
		
		if (const FEssence* RowPtr = EssenceTable->FindRow<FEssence>(RowName, TEXT("Loading Essences")); 
			RowPtr)
		{
			Essences.Add(*RowPtr); 
		}
	}
}

void UVendingMachineSlot::ScrollRecipe()
{
	if (Recipes.IsEmpty()) 
	{
		DebugHelper::LogMessage(7, FColor::Red, "Recipes are empty");
		return;
	}
	
	CurrentRecipeIndex = (CurrentRecipeIndex + 1) % Recipes.Num();
	CurrentSelectedRecipe = Recipes[CurrentRecipeIndex];
	
	
}

void UVendingMachineSlot::ScrollEssence()
{
	if (Essences.IsEmpty())
	{
		return;
	}
	
	CurrentEssenceIndex = (CurrentEssenceIndex + 1) % Essences.Num();
	RefreshEssence();
}

void UVendingMachineSlot::ScrollCasing()
{
	if (Casings.IsEmpty())
	{
		return;
	}
	
	CurrentCasingIndex = (CurrentCasingIndex + 1) % Casings.Num();
	CurrentSelectedCasing = Casings[CurrentCasingIndex];
}

void UVendingMachineSlot::RefreshRecipe()
{
	if (!Recipes.IsValidIndex(CurrentRecipeIndex))
	{
		return;
	}
	
	
	//RecipeName->SetText(FText::FromString(CurrentSelectedRecipe.GetName(CurrentSelectedRecipe.RequiredBlueprintType)));
}

void UVendingMachineSlot::AddRecipe()
{
	if (!Recipes.IsValidIndex(CurrentRecipeIndex))
	{
		return;
	}
	
	const FRecipe& Recipe = Recipes[CurrentRecipeIndex];
	Instance->GetCurrentPlayer()->GetInventoryManager()->RecipeUnlocked.Broadcast(Recipe.RequiredBlueprintType);
}

void UVendingMachineSlot::IncreaseEssence()
{
	if (!Essences.IsValidIndex(CurrentEssenceIndex))
	{
		return;
	}
	
	Essences[CurrentEssenceIndex].Quantity++;
	
	if (Essences[CurrentEssenceIndex].Quantity >= 100)
	{
		Essences[CurrentEssenceIndex].Quantity = 100;
	}
}

void UVendingMachineSlot::DecreaseEssence()
{
	if (!Essences.IsValidIndex(CurrentEssenceIndex))
	{
		return;
	}
	
	Essences[CurrentEssenceIndex].Quantity--;
	
	if (Essences[CurrentEssenceIndex].Quantity <= 0)
	{
		Essences[CurrentEssenceIndex].Quantity = 0;
	}
}

void UVendingMachineSlot::AddEssence()
{
	if (!Essences.IsValidIndex(CurrentEssenceIndex))
	{
		return;
	}
	
	Instance->GetCurrentPlayer()->GetInventoryManager()->AddEssence(Essences[CurrentEssenceIndex]);
}

void UVendingMachineSlot::IncreaseCasingQuantity()
{
	if (!Casings.IsValidIndex(CurrentCasingIndex))
	{
		return;
	}
	
	CasingAmount++;
	
	Casings[CurrentCasingIndex].SetQuantity(CasingAmount);
	
	if (Casings[CurrentCasingIndex].GetQuantity() > 100)
	{
		Casings[CurrentCasingIndex].SetQuantity(100);
	}
}

void UVendingMachineSlot::DecreaseCasingQuantity()
{
	if (!Casings.IsValidIndex(CurrentCasingIndex))
	{
		return;
	}
	
	CasingAmount--;
	
	Casings[CurrentCasingIndex].SetQuantity(CasingAmount);
	
	if (Casings[CurrentCasingIndex].GetQuantity() <= 0)
	{
		Casings[CurrentCasingIndex].SetQuantity(0);
	}
}


void UVendingMachineSlot::AddCasing()
{
	const FCasing& Casing = Casings[CurrentCasingIndex];
	Instance->GetInventory().CasingsStored.Add(Casing.GetName(), Casing);
	
	FBullet Bullet;
	Bullet.SetBulletData(Indifference);
	Bullet.SetQuantity(CasingAmount);
	Instance->GetInventory().BulletsStored.Add(Bullet.GetBulletData()->Type, Bullet);
}

void UVendingMachineSlot::RefreshEssence()
{
	if (!Essences.IsValidIndex(CurrentEssenceIndex))
	{
		return;
	}
	
	CurrentSelectedEssence = Essences[CurrentEssenceIndex];
	EssenceName->SetText(FText::FromString(CurrentSelectedEssence.GetName(CurrentSelectedEssence.EssenceType)));
}

void UVendingMachineSlot::Refresh()
{
	RefreshEssence();
	RefreshRecipe();
}
