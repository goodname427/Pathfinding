// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePawn.h"
#include "Component/CollectorComponent.h"
#include "CollectorPawn.generated.h"

class AResourcePawn;
class UTransportCommandComponent;
class UCollectCommandComponent;

UCLASS()
class PATHFINDING_API ACollectorPawn : public AMovablePawn
{
	GENERATED_BODY()

public:
	ACollectorPawn();

public:
	virtual UCommandComponent* ResolveRequestWithoutName_Implementation(const FTargetRequest& Request) override;

protected:
	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectCommandComponent* CollectCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UTransportCommandComponent* TransportCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectorComponent* CollectorComponent;
};
