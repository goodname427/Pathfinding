// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingPawn.h"
#include "Command/ProduceCommandComponent.h"
#include "KitchenPawn.generated.h"

UCLASS()
class PATHFINDING_API AKitchenPawn : public ABuildingPawn
{
	GENERATED_BODY()

public:
	AKitchenPawn();
	
protected:
	UPROPERTY(Category = "Produce", VisibleDefaultsOnly, BlueprintReadOnly)
	UProduceCommandComponent* ProduceCommandComponent;
};
