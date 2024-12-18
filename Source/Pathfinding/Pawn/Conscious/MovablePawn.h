// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConsciousPawn.h"
#include "Movement/ConsciousPawnMovementComponent.h"
#include "MovablePawn.generated.h"

UCLASS()
class PATHFINDING_API AMovablePawn : public AConsciousPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMovablePawn();

public:
	virtual UPawnMovementComponent* GetMovementComponent() const override { return MovementComponent; };

	virtual UMoveCommandComponent* GetMoveCommandComponent() const override { return MoveCommandComponent; }

protected:
	UPROPERTY(Category = "Movable", VisibleAnywhere)
	UPawnMovementComponent* MovementComponent;

	UPROPERTY(Category = "Movable", VisibleAnywhere)
	UMoveCommandComponent* MoveCommandComponent;
};
