// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"


FString FCasing::GetName() const
{
	return Name;
}

UTexture2D* FCasing::GetIcon() const
{
	return Icon;
}

int32 FCasing::GetQuantity() const
{
	return Quantity;
}

void FCasing::SetQuantity(const int32& NewQuantity)
{
	this->Quantity = NewQuantity;
}

ECasingType& FCasing::GetType()
{
	return RequiredCasingType;
}

void FCasing::SetType(const ECasingType& NewType)
{
	RequiredCasingType = NewType;
}

bool FCasing::IsValid() const
{
	return Icon != nullptr;
}
