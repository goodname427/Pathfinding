// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPawn.h"

#include "Command/GatherCommandComponent.h"
#include "Command/SpawnCommandComponent.h"


// Sets default values
ABuildingPawn::ABuildingPawn()
{
	ConsciousData.AllowedCreateMethod = static_cast<uint32>(EAllowedCreateMethod::Build);
	ConsciousData.ResourcesToAmount = {{EResourceType::Coin, 1}};
}
