// Fill out your copyright notice in the Description page of Project Settings.


#include "GoldBulletDisplayer.h"

void UGoldBulletDisplayer::NativeConstruct()
{
    Super::NativeConstruct();

    GoldBullet = {
        Dummy,
        Dummy2
       
    };
}

TArray<UBulletBottonItem *> UGoldBulletDisplayer::GetBullet() const
{
    return GoldBullet;
}

