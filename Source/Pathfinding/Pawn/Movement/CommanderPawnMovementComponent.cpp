// Fill out your copyright notice in the Description page of Project Settings.


#include "CommanderPawnMovementComponent.h"

#include "PFUtils.h"
#include "Components/BoxComponent.h"
#include "Engine/LevelBounds.h"

UCommanderPawnMovementComponent::UCommanderPawnMovementComponent()
{
	MaxSpeed = 7200.f;
	Acceleration = 36000.f;
	Deceleration = 72000.f;
	TurningBoost = 16.0f;
	bPositionCorrected = false;

	ResetMoveState();
}

void UCommanderPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	if (ShouldSkipUpdate(DeltaTime))
	{
		return;
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PawnOwner || !UpdatedComponent)
	{
		return;
	}

	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		// apply input for local players but also for AI that's not following a navigation path at the moment
		if (Controller->IsLocalPlayerController() == true || Controller->IsFollowingAPath() == false ||
			bUseAccelerationForPaths)
		{
			ApplyControlInputToVelocity(DeltaTime);
		}
		// if it's not player controller, but we do have a controller, then it's AI
		// (that's not following a path) and we need to limit the speed
		else if (IsExceedingMaxSpeed(MaxSpeed) == true)
		{
			Velocity = Velocity.GetUnsafeNormal() * MaxSpeed;
		}

		LimitWorldBounds();
		bPositionCorrected = false;

		// Move actor
		FVector Delta = Velocity * DeltaTime;

		if (!Delta.IsNearlyZero(1e-6f))
		{
			const FVector OldLocation = UpdatedComponent->GetComponentLocation();
			const FQuat Rotation = UpdatedComponent->GetComponentQuat();

			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(Delta, Rotation, true, Hit);

			if (Hit.IsValidBlockingHit())
			{
				HandleImpact(Hit, DeltaTime, Delta);
				// Try to slide the remaining distance along the surface.
				SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
			}

			// Update velocity
			// We don't want position changes to vastly reverse our direction (which can happen due to penetration fixups etc)
			if (!bPositionCorrected)
			{
				const FVector NewLocation = UpdatedComponent->GetComponentLocation();
				Velocity = ((NewLocation - OldLocation) / DeltaTime);
			}
		}

		// Finalize
		UpdateComponentVelocity();
	}
};

bool UCommanderPawnMovementComponent::LimitWorldBounds()
{
	// AWorldSettings* WorldSettings = PawnOwner ? PawnOwner->GetWorldSettings() : NULL;
	// if (!WorldSettings || !WorldSettings->bEnableWorldBoundsChecks || !UpdatedComponent)
	// {
	// 	return false;
	// }
	//
	// if (CurrentLocation.Z < WorldSettings->KillZ)
	// {
	// 	Velocity.Z = FMath::Min(GetMaxSpeed(), WorldSettings->KillZ - CurrentLocation.Z + 2.0f);
	// 	return true;
	// }
	NULL_CHECK_RET(PawnOwner, false);

	const FVector CurrentLocation = UpdatedComponent->GetComponentLocation();

	if (const UWorld* World = PawnOwner->GetWorld())
	{
		if (const ULevel* Level = World->GetCurrentLevel())
		{
			const TWeakObjectPtr<ALevelBounds> LevelBounds = Level->LevelBoundsActor;
			if (LevelBounds.IsValid())
			{
				//const UBoxComponent* BoxComponent = LevelBounds->BoxComponent;
				//const FTransform& BoxTransform = BoxComponent->GetComponentTransform();
				const FBox Box = LevelBounds->GetComponentsBoundingBox();
				// DEBUG_MESSAGE(TEXT("%s"), *Box.ToString());

				// if (!Box.IsInside(CurrentLocation))
				// {
				bool HasModified = false;
				
				if (CurrentLocation.X > Box.Max.X)
				{
					Velocity.X = FMath::Min(Velocity.X, 0.0f);
					HasModified = true;
				}
				else if (CurrentLocation.X < Box.Min.X)
				{
					Velocity.X = FMath::Max(Velocity.X, 0.0f);
					HasModified = true;
				}

				if (CurrentLocation.Y > Box.Max.Y)
				{
					Velocity.Y = FMath::Min(Velocity.Y, 0.0f);
					HasModified = true;
				}
				else if (CurrentLocation.Y < Box.Min.Y)
				{
					Velocity.Y = FMath::Max(Velocity.Y, 0.0f);
					HasModified = true;
				}

				if (CurrentLocation.Z > Box.Max.Z)
				{
					Velocity.Z = FMath::Min(Velocity.Z, 0.0f);
					HasModified = true;
				}
				else if (CurrentLocation.Z < Box.Min.Z)
				{
					Velocity.Z = FMath::Max(Velocity.Z, 0.0f);
					HasModified = true;
				}

				return HasModified;
				// }
			}
		}
	}


	return false;
}

void UCommanderPawnMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
{
	const FVector ControlAcceleration = GetPendingInputVector().GetClampedToMaxSize(1.f);

	const float AnalogInputModifier = (ControlAcceleration.SizeSquared() > 0.f ? ControlAcceleration.Size() : 0.f);
	const float MaxPawnSpeed = GetMaxSpeed() * AnalogInputModifier;
	const bool bExceedingMaxSpeed = IsExceedingMaxSpeed(MaxPawnSpeed);

	if (AnalogInputModifier > 0.f && !bExceedingMaxSpeed)
	{
		// Apply change in velocity direction
		if (Velocity.SizeSquared() > 0.f)
		{
			// Change direction faster than only using acceleration, but never increase velocity magnitude.
			const float TimeScale = FMath::Clamp(DeltaTime * TurningBoost, 0.f, 1.f);
			Velocity = Velocity + (ControlAcceleration * Velocity.Size() - Velocity) * TimeScale;
		}
	}
	else
	{
		// Dampen velocity magnitude based on deceleration.
		if (Velocity.SizeSquared() > 0.f)
		{
			const FVector OldVelocity = Velocity;
			const float VelSize = FMath::Max(Velocity.Size() - FMath::Abs(Deceleration) * DeltaTime, 0.f);
			Velocity = Velocity.GetSafeNormal() * VelSize;

			// Don't allow braking to lower us below max speed if we started above it.
			if (bExceedingMaxSpeed && Velocity.SizeSquared() < FMath::Square(MaxPawnSpeed))
			{
				Velocity = OldVelocity.GetSafeNormal() * MaxPawnSpeed;
			}
		}
	}

	// Apply acceleration and clamp velocity magnitude.
	const float NewMaxSpeed = (IsExceedingMaxSpeed(MaxPawnSpeed)) ? Velocity.Size() : MaxPawnSpeed;
	Velocity += ControlAcceleration * FMath::Abs(Acceleration) * DeltaTime;
	Velocity = Velocity.GetClampedToMaxSize(NewMaxSpeed);

	ConsumeInputVector();
}

bool UCommanderPawnMovementComponent::ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit,
                                                             const FQuat& NewRotationQuat)
{
	bPositionCorrected |= Super::ResolvePenetrationImpl(Adjustment, Hit, NewRotationQuat);
	return bPositionCorrected;
}
