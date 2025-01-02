// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "CommanderPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UCommanderPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UCommanderPawnMovementComponent();

private:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual float GetMaxSpeed() const override { return MaxSpeed; }

protected:
	virtual bool
	ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation) override;

	/** Update Velocity based on input. Also applies gravity. */
	virtual void ApplyControlInputToVelocity(float DeltaTime);

	/** Prevent Pawn from leaving the world bounds (if that restriction is enabled in WorldSettings) */
	virtual bool LimitWorldBounds();
	
public:
	/** Maximum velocity magnitude allowed for the controlled Pawn. */
	UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite)
	float MaxSpeed;

	/** Acceleration applied by input (rate of change of velocity) */
	UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite)
	float Acceleration;

	/** Deceleration applied when there is no input (rate of change of velocity) */
	UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite)
	float Deceleration;

	/**
	 * Setting affecting extra force applied when changing direction, making turns have less drift and become more responsive.
	 * Velocity magnitude is not allowed to increase, that only happens due to normal acceleration. It may decrease with large direction changes.
	 * Larger values apply extra force to reach the target direction more quickly, while a zero value disables any extra turn force.
	 */
	UPROPERTY(Category = "Movement", EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0", UIMin = "0"))
	float TurningBoost;

protected:
	/** Set to true when a position correction is applied. Used to avoid recalculating velocity when this occurs. */
	UPROPERTY(Transient)
	uint32 bPositionCorrected : 1;
};
