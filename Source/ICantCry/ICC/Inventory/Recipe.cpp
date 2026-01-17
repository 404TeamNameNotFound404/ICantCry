// Fill out your copyright notice in the Description page of Project Settings.


#include "Recipe.h"

FString FRecipe::GetName(const ERecipeType& RecipeType) const
{
	switch (RecipeType)
	{
	case ERecipeType::Gold:
		return FString("Gold");
	case ERecipeType::Anger:
		return FString("Anger");
	case ERecipeType::Sadness:
		return FString("Sadness");
	case ERecipeType::Anxiety:
		return FString("Anxiety");
	case ERecipeType::Shame:
		return FString("Shame");
	case ERecipeType::Base:
		return FString("Base");
	case ERecipeType::Disgust:
		return FString("Disgust");
	case ERecipeType::Joy:
		return FString("Joy");
	case ERecipeType::JoyEv:
		return FString("Joy (Ev)");
	case ERecipeType::Fear:
		return FString("Fear");
	case ERecipeType::FearEv:
		return FString("Fear (Ev)");
	case ERecipeType::Calm:
		return FString("Calm");
	case ERecipeType::AngerEv:
		return FString("Anger (Ev)");
	case ERecipeType::CalmEv:
		return FString("Calm (Ev)");
	case ERecipeType::Jealousy:
		return FString("Jealousy");
	case ERecipeType::JealousyEv:
		return FString("Jealousy (Ev)");
	case ERecipeType::Indifference:
		return FString("Indifference");
	default:
		return FString("None");
	}
}

FString FRecipe::GetCaseType(const ECasingType& Type) const
{
	switch (Type)
	{
	case ECasingType::Base:
		return FString("Base");
	case ECasingType::Gold:
		return FString("Gold");
	default:
		return FString("None");
	}
}

FString FRecipe::GetEssencesName() const
{
	for (FEssence E : RequiredEssences)
	{
		return E.GetName(E.EssenceType);
	}

	return "";
}



