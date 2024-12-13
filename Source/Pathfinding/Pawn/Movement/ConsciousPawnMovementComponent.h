#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ConsciousPawnMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FConsciousMoveData
{
	GENERATED_BODY()

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FRotator Rotation;
};

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UConsciousPawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	UConsciousPawnMovementComponent();

private:
	//Begin UActorComponent Interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	//End UActorComponent Interface

public:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastUpdate(const FConsciousMoveData& MoveData);

protected:
	UPROPERTY(Category = "Network", EditAnywhere)
	float NetworkTickInterval;

	float LastUpdateTime;

public:
	virtual float GetMaxSpeed() const override { return MaxSpeed; }

protected:
	virtual bool
	ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit, const FQuat& NewRotation) override;

public:
	/** Maximum velocity magnitude allowed for the controlled Pawn. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PFCameraPawnMovement)
	float MaxSpeed;

	/** Acceleration applied by input (rate of change of velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PFCameraPawnMovement)
	float Acceleration;

	/** Deceleration applied when there is no input (rate of change of velocity) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PFCameraPawnMovement)
	float Deceleration;

	/**
	 * Setting affecting extra force applied when changing direction, making turns have less drift and become more responsive.
	 * Velocity magnitude is not allowed to increase, that only happens due to normal acceleration. It may decrease with large direction changes.
	 * Larger values apply extra force to reach the target direction more quickly, while a zero value disables any extra turn force.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FloatingPawnMovement, meta = (ClampMin = "0", UIMin = "0"))
	float TurningBoost;

protected:
	/** Update Velocity based on input. Also applies gravity. */
	virtual void ApplyControlInputToVelocity(float DeltaTime);

	/** Prevent Pawn from leaving the world bounds (if that restriction is enabled in WorldSettings) */
	virtual bool LimitWorldBounds();

	/** Set to true when a position correction is applied. Used to avoid recalculating velocity when this occurs. */
	UPROPERTY(Transient)
	uint32 bPositionCorrected : 1;
};
