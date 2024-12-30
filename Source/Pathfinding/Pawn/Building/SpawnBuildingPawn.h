// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingPawn.h"
#include "Command/GatherCommandComponent.h"
#include "Command/SpawnCommandComponent.h"
#include "SpawnBuildingPawn.generated.h"

UCLASS()
class PATHFINDING_API ASpawnBuildingPawn : public ABuildingPawn
{
	GENERATED_BODY()

public:
	ASpawnBuildingPawn();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	bool HasSpawnCommand() const;
	
protected:
	UPROPERTY(Category = "SpawnBuilding", VisibleAnywhere)
	UGatherCommandComponent* GatherCommandComponent;

	UPROPERTY(Category = "SpawnBuilding", VisibleAnywhere)
	UStaticMeshComponent* GatherFlagMeshComponent;

public:
	virtual void OnSelected(class ACommanderPawn* SelectCommander) override;
	virtual void OnDeselected() override;

protected:
	virtual UCommandComponent* ResolveRequestWithoutName_Implementation(const FTargetRequest& Request) override;
};
