// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingPawn.h"
#include "KitchenPawn.generated.h"

UCLASS()
class PATHFINDING_API AKitchenPawn : public ABuildingPawn
{
	GENERATED_BODY()

public:
	AKitchenPawn();

protected:
	virtual void BeginPlay() override;

	virtual void OnDied() override;

public:
	UFUNCTION(BlueprintCallable)
	int32 GetProducedFoodPerSecond() const { return ProducedFoodPerSecond; }

protected:
	UPROPERTY(Category = "Kitchen", EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int32 ProducedFoodPerSecond;

private:
	FTimerHandle ProducedFoodTimerHandle;
};
