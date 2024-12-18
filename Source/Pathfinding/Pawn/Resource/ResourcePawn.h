// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPawn.h"
#include "Conscious/CollectorPawn.h"
#include "ResourcePawn.generated.h"

UCLASS()
class PATHFINDING_API AResourcePawn : public APFPawn
{
	GENERATED_BODY()

public:
	AResourcePawn();

public:
	void CollectBy(ACollectorPawn* CollectorPawn);

	EResourceType GetResourceType() const { return ResourceType; };

	int32 GetResourcePoint() const { return ResourcePoint; };

protected:
	UPROPERTY(Category = "Resource", EditAnywhere)
	int32 ResourcePoint;
	
	UPROPERTY(Category = "Resource", EditAnywhere)
	EResourceType ResourceType;
};
