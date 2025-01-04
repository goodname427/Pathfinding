// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsciousPawn.h"
#include "BuildingPawn.generated.h"

UCLASS()
class PATHFINDING_API ABuildingPawn : public AConsciousPawn
{
	GENERATED_BODY()

public:
	ABuildingPawn();

public:
	bool IsLocationValid(const UObject* WorldContextObject, const FVector& Location) const;
};
