// Fill out your copyright notice in the Description page of Project Settings.

#include "StandardBulletDisplayer.h"

#include "ICantCry/ICC/Actors/Player/ICC_Player.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UStandardBulletDisplayer::NativeConstruct()
{
    Super::NativeConstruct();

    // StandardBullet = {
    //     AngerDV,
    //     AngerEV,
    //     FearDV,
    //     FearEV,
    //     DisgustDV,
    //     DisgustEV,
    //     SadnessDV,
    //     SadnessEV,
    //     JoyDV,
    //     JoyEV,
    //     NoneBullet,
    //     CalmEV,
    //     JealousyDV,
    //     JealousyEV,
    //     Anxiety,
    //     Shame
    // };
    //
    // RecipeUnlocked.AddUObject(this, &UStandardBulletDisplayer::Unlock);
}

void UStandardBulletDisplayer::NativeDestruct()
{
    Super::NativeDestruct();

    GameInstance->GetCurrentPlayer()->GetInventoryManager()->RecipeUnlocked.RemoveAll(this);
    bBound = false;
}

TArray<UBulletBottonItem*> UStandardBulletDisplayer::GetBullet() const
{
    return StandardBullet;
}

void UStandardBulletDisplayer::Init(UInventoryHUD* Hud)
{
    GameInstance = Cast<UICantCryGameInstance>(GetGameInstance());
    
    StandardBullet = {
        AngerDV,
        AngerEV,
        FearDV,
        FearEV,
        DisgustDV,
        DisgustEV,
        SadnessDV,
        SadnessEV,
        JoyDV,
        JoyEV,
        NoneBullet,
        CalmEV,
        JealousyDV,
        JealousyEV,
        Anxiety,
        Shame
    };

    if (!bBound)
    {
        FTimerHandle DelayHandle;
        GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
        {
            GameInstance->GetCurrentPlayer()->GetInventoryManager()->RecipeUnlocked.AddUObject(this, &UStandardBulletDisplayer::Unlock);
            bBound = true;
            
            for (TArray<FRecipe>& RecipesAlreadyOwned = GameInstance->GetInventory().Recipes; 
                const FRecipe& Recipe : RecipesAlreadyOwned)
            {
                this->Unlock(Recipe.RequiredBlueprintType); 
            }
            
        }, 0.65f, false);
    }
    
    
    for (UBulletBottonItem *Btn : StandardBullet)
    {
        if (!Btn)
        {
            continue;
        }

        Btn->SetOwner(Hud);
    }
}

// qua non faccio altro che vedere il tipo di blueprint e confrontarlo con quello richiesto
// se è quello in teoria dovrebbe sbloccare il bottone corrispondente al tipo

void UStandardBulletDisplayer::Unlock(ERecipeType Type)
{
    if (StandardBullet.IsEmpty())
    {
        DebugHelper::LogError("Empty");
        return;
    }
    
    
    for (UBulletBottonItem *Btn : StandardBullet)
    {
        if (!Btn)
        {
            continue;
        }

        if (Btn->GetBlueprint().RequiredBlueprintType == Type)
        {
            DebugHelper::LogSuccess("Unlocked " + Btn->GetBlueprint().GetName(Type));
            Btn->SetUnlocked(true);
            GameInstance->GetInventory().Recipes.Add(Btn->GetBlueprint());
        }
    }

    Refresh();
}

void UStandardBulletDisplayer::Refresh()
{
    for (UBulletBottonItem *Btn : StandardBullet)
    {
        if (!Btn)
        {
            continue;
        }

        if (Btn->GetIsUnlocked())
        {
            Btn->Show();
        }
        else
        {
            Btn->Hide();
        }
    }
}
