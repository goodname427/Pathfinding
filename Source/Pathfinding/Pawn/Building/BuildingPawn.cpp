// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPawn.h"

#include "PFUtils.h"
#include "Command/BuildingCommandComponent.h"
#include "Command/GatherCommandComponent.h"
#include "Command/SpawnCommandComponent.h"


// Sets default values
ABuildingPawn::ABuildingPawn(): bInBuilding(false)
{
	ConsciousData.AllowedCreateMethod = TO_FLAG(EAllowedCreateMethod::Build);
	ConsciousData.ResourceCost = {{EResourceType::Coin, 1}};
}

void ABuildingPawn::EndBuilding_Implementation()
{
	bInBuilding = false;

	if (HasAuthority())
	{
		if (BuildingCommandComponent)
		{
			BuildingCommandComponent->DestroyComponent();
		}
	}

	RefreshCommandList();
}

void ABuildingPawn::BeginBuilding_Implementation()
{
	bInBuilding = true;

	if (HasAuthority())
	{
		BuildingCommandComponent = AddNewCommand<UBuildingCommandComponent>();
		if (BuildingCommandComponent)
		{
			Receive(FTargetRequest(BuildingCommandComponent));
		}
		else
		{
			Destroy();
		}
	}
	
	CommandList.Empty();
}
