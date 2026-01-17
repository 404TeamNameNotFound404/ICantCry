// Fill out your copyright notice in the Description page of Project Settings.


#include "StandardBulletDisplayer.h"
#include "ICantCry/ICC/Debug/DebugHelper.h"

void UStandardBulletDisplayer::NativeConstruct()
{
    Super::NativeConstruct();

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


    for(UBulletBottonItem* Btn : StandardBullet)
    {
        DebugHelper::LogWarning(Btn->GetName() + "  IsUnlocked   " + FString::FromInt(Btn->GetIsUnlocked()));
    }


}

TArray<UBulletBottonItem*> UStandardBulletDisplayer::GetBullet() const
{
    return StandardBullet;
}

void UStandardBulletDisplayer::Refresh()
{

    DebugHelper::LogSuccess("Funziona REFRESH");

    for (UBulletBottonItem *Btn : StandardBullet)
    {
        if (!Btn)
            continue;

        Btn->SetUnlocked(Btn->GetIsUnlocked());

        if (Btn->GetIsUnlocked())
        {
            
            Btn->Show();
            DebugHelper::LogWarning("Funziona SHOW");
        }
    }
}
