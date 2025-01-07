// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameSettings.h"

const UCamp* UPFGameSettings::GetRandomlyCamp() const
{
	if (CampClasses.Num() == 0)
	{
		return nullptr;
	}

	return CampClasses[FMath::Rand() % CampClasses.Num()].GetDefaultObject();
}

UMaterialInterface* UPFGameSettings::LoadPawnFlagMaterial() const
{
	return Cast<UMaterialInterface>(PawnFlagMaterial.TryLoad());
}

UMaterialInterface* UPFGameSettings::LoadBuildingTranslucentMaterial() const
{
	return Cast<UMaterialInterface>(BuildingTranslucentMaterial.TryLoad());
}
