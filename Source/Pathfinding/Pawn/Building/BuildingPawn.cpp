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

bool ABuildingPawn::IsLocationValid(const UObject* WorldContextObject, const FVector& Location) const
{
	const UWorld* World = WorldContextObject->GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	FVector Origin;
	FVector BoxExtent;
	GetActorBounds(true, Origin, BoxExtent);

	FHitResult Hit;
	World->SweepSingleByChannel(
		Hit,
		Location ,
		Origin,
		FQuat::Identity,
		ECC_Camera,
		FCollisionShape::MakeBox(BoxExtent)
	);

	return !Hit.bBlockingHit;
}
