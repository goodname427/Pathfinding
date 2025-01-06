// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpawnBuildingPawn.h"
#include "Movable/CollectorPawn.h"
#include "BaseCampPawn.generated.h"

UCLASS()
class PATHFINDING_API ABaseCampPawn : public ASpawnBuildingPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseCampPawn();

public:
	void TransportBy(UCollectorComponent* Collector);
};
