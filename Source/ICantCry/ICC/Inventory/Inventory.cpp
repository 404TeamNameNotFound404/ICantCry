// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory.h"
#include "ICantCry/ICC/UI/CraftingHUD.h"


void FInventory::AddToInventory(EItemType ItemType, FBullet Bullet, const TArray<FEssence>& ItemEssences,
	FRecipe Recipe, int32 Quantity)
{
	bool bWasAdded = false;
    
	for (FInventoryItem& Item : Items)
	{
		if (Item.ItemType == ItemType && CompareItemData(Item, Bullet, ItemEssences, Recipe))
		{
			Item.Quantity += Quantity;
			bWasAdded = true;
			break;
		}
	}

	if (!bWasAdded)
	{
		FInventoryItem NewInventoryItem;
		NewInventoryItem.ItemType = ItemType;
		NewInventoryItem.Bullet = Bullet;
		NewInventoryItem.Essences = ItemEssences;  
		NewInventoryItem.Recipe = Recipe;
		NewInventoryItem.Quantity = Quantity;
		Items.Add(NewInventoryItem);
	}
}

void FInventory::AddEssenceInMap(const EEssenceType& EssenceType, const int32& Quantity)
{
	if (EssencesInInventory.Contains(EssenceType))
	{
		EssencesInInventory[EssenceType] += Quantity;
	}
	else
	{
		EssencesInInventory.Add(EssenceType, Quantity);
	}
}


void FInventory::RemoveEssence(EEssenceType EssenceType, int32 Quantity)
{
	for (int32 i = 0; i < Essences.Num(); ++i)
	{
		if (Essences[i].EssenceType == EssenceType)
		{
			Essences[i].Quantity = FMath::Max(0, Essences[i].Quantity - Quantity);
            
			if (Essences[i].Quantity == 0)
			{
				Essences.RemoveAt(i);
			}

			return;
		}
	}
}

void FInventory::RemoveFromInventory(EItemType ItemType, FBullet Bullet, const TArray<FEssence>& ItemEssences,
	FRecipe Recipe, int32 Quantity)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].ItemType == ItemType && CompareItemData(Items[i], Bullet, ItemEssences, Recipe))
		{
			Items[i].Quantity -= Quantity;
			if (Items[i].Quantity <= 0)
			{
				Items.RemoveAt(i);
			}

			return;
		}   
	}
}

void FInventory::RemoveCasing(ECasingType CasingType, int32 Quantity)
{
	switch (CasingType)
	{
	case ECasingType::Base:
		EmptyCasingCount = FMath::Max(0, EmptyCasingCount - Quantity);
		break;
	case ECasingType::Gold:
		GoldCasingCount = FMath::Max(0, GoldCasingCount - Quantity);
		break;
	}
}

void FInventory::RemoveBullet(UBulletData* BulletData, int32 Amount)
{
	for (int32 i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Bullet.GetBulletData() && Items[i].Bullet.GetBulletData()->ID == BulletData->ID)
		{
			Items[i].Quantity = FMath::Max(0, Items[i].Quantity - Amount);

			if (Items[i].Quantity == 0)
			{
				Items.RemoveAt(i);
			}

			return;
            
		}   
	}
}

bool FInventory::CompareItemData(const FInventoryItem& Item, const FBullet& Bullet,
	const TArray<FEssence>& ItemEssences, const FRecipe& Recipe) const
{
	if (!(Item.Bullet == Bullet) || !(Item.Recipe == Recipe))
	{
		return false;
	} 

	if (Item.Essences.Num() != Essences.Num())
	{
		return false;
	}
            
	TArray<FEssence> A = Item.Essences;
	TArray<FEssence> B = Essences;

	A.Sort([](const FEssence& X, const FEssence& Y) { return (uint8)X.EssenceType < (uint8)Y.EssenceType; });
	B.Sort([](const FEssence& X, const FEssence& Y) { return (uint8)X.EssenceType < (uint8)Y.EssenceType; });

	for (int32 i = 0; i < A.Num(); ++i)
	{
		if (!(A[i] == B[i]))
			return false;
	}   

	return true;
}

void FInventory::AddCraftedBullet(FBullet& Bullet)
{
	
    if (!Bullet.GetBulletData())
    {
        UE_LOG(LogTemp, Error, TEXT("AddCraftedBullet: Bullet data is null!"));
        return;
    }
	
    TEnumAsByte<EBulletType> BulletType = Bullet.GetBulletData()->Type;

    FBullet* ExistingBullet = BulletsStored.Find(BulletType);
	//FBullet& FindBullet = BulletsStored.FindOrAdd(BulletType);
    
    if (ExistingBullet && ExistingBullet->IsValid())
    {
        const int32 CurrentQuantity = ExistingBullet->GetQuantity();
        ExistingBullet->SetQuantity(CurrentQuantity + 1);
    }
    else
    {
        FBullet NewBullet = Bullet;
        NewBullet.SetQuantity(1);  
        BulletsStored.Add(BulletType, NewBullet);
    }

}

int32 FInventory::GetEssenceQuantity(EEssenceType EssenceType) const
{
	for (const FEssence& Essence : Essences)
	{
		if (Essence.EssenceType == EssenceType)
		{
			return Essence.Quantity;
		}
	}
	return 0;
}


const TArray<FInventoryItem>& FInventory::GetAllItems() const
{
	return Items;
}

void FInventory::SetSelectedRecipe(const FRecipe& SelectedRecipe)
{
	DebugHelper::LogMessage(4, FColor::Purple, "Selected Recipe Quantity " + FString::FromInt(SelectedRecipe.Requirements->EssenceQuantity) + "\nCasing Quantity " + FString::FromInt(SelectedRecipe.Requirements->CasingQuantity));
	CurrentRecipe = SelectedRecipe;
	CurrentRecipe.ResultBullet.SetBulletData(nullptr);
	CurrentRecipe.ResultBullet.SetBulletData(SelectedRecipe.ResultBullet.GetBulletData());
	DebugHelper::LogSuccess(SelectedRecipe.ResultBullet.GetBulletData()->BulletName);
}

FRecipe& FInventory::GetSelectedRecipe()
{
	return CurrentRecipe;
}










