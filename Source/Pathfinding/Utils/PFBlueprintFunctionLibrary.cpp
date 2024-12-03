// Fill out your copyright notice in the Description page of Project Settings.


#include "PFBlueprintFunctionLibrary.h"

const UPFGameSettings* UPFBlueprintFunctionLibrary::GetPFGameSettings()
{
	return GetDefault<UPFGameSettings>();
}
