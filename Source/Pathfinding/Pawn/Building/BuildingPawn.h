// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsciousPawn.h"
#include "Command/BuildingCommandComponent.h"
#include "BuildingPawn.generated.h"

UCLASS()
class PATHFINDING_API ABuildingPawn : public AConsciousPawn
{
	GENERATED_BODY()

public:
	ABuildingPawn();
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsInBuilding() const { return bInBuilding; }
	
	UFUNCTION(NetMulticast, Reliable)
	void BeginBuilding();

	UFUNCTION(NetMulticast, Reliable)
	void EndBuilding();

private:
	bool bInBuilding;

	UPROPERTY()
	UBuildingCommandComponent* BuildingCommandComponent;
};
