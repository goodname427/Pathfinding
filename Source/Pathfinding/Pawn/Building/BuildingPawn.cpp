// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPawn.h"

#include "Command/GatherCommandComponent.h"
#include "Command/SpawnCommandComponent.h"


// Sets default values
ABuildingPawn::ABuildingPawn(): bInBuilding(false)
{
	ConsciousData.AllowedCreateMethod = TO_FLAG(EAllowedCreateMethod::Build);
	ConsciousData.ResourceCost = {{EResourceType::Coin, 1}};
}

void ABuildingPawn::SetupInBuilding()
{
	bInBuilding = true;
}
