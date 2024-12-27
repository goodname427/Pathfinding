// Fill out your copyright notice in the Description page of Project Settings.


#include "Camp.h"

const TArray<UCamp*>& UCamp::GetAllCamps()
{
	static TArray<UCamp*> AllCamps;
	static bool HasInitialized = false;

	if (HasInitialized)
	{
		return AllCamps;
	}

	TArray<UClass*> CampClasses;
	GetDerivedClasses(UCamp::StaticClass(), CampClasses, true);
	for (const UClass* CampClass : CampClasses)
	{
		AllCamps.Add(Cast<UCamp>(CampClass->GetDefaultObject()));
	}

	HasInitialized = true;
	return AllCamps;
}

const UCamp* UCamp::GetRandomlyCamp()
{
	const TArray<UCamp*> AllCamps = GetAllCamps();

	if (AllCamps.Num() == 0)
	{
		return nullptr;
	}

	return AllCamps[FMath::Rand() % AllCamps.Num()];
}
