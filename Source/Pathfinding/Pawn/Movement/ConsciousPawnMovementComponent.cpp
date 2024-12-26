// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawnMovementComponent.h"

#include "PFUtils.h"
#include "Components/CapsuleComponent.h"

UConsciousPawnMovementComponent::UConsciousPawnMovementComponent()
{
	MaxSpeed = 2400.f;
	Acceleration = 12000.f;
	Deceleration = 24000.f;
	TurningBoost = 8.0f;
	bPositionCorrected = false;

	ResetMoveState();
}

void UConsciousPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
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
	}
	else
	{
		Velocity = FVector::ZeroVector;
	}

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

		// Improved ground and step detection
		const FVector MovedLocation = UpdatedComponent->GetComponentLocation();
		
		// Get capsule component and its properties
		UCapsuleComponent* CapsuleComponent = Cast<UCapsuleComponent>(UpdatedComponent);
		if (!CapsuleComponent)
		{
			return;
		}

		const float CapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
		const float CapsuleHalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();

		// Setup sweep parameters based on capsule size
		FCollisionShape SweepShape = FCollisionShape::MakeSphere(CapsuleRadius * 0.5f);  // 使用胶囊体半径的一半作为扫描球体
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(PawnOwner);

		// Forward ground detection to prevent floating at edges
		FVector ForwardLocation = MovedLocation + (Velocity.GetSafeNormal2D() * ForwardCheckDistance);

		// Check points for ground detection
		TArray<FVector> CheckPoints = {
			MovedLocation,                    // Current position
			ForwardLocation,                  // Forward position
			MovedLocation + FVector(-CapsuleRadius,0,0), // Left
			MovedLocation + FVector(CapsuleRadius,0,0),  // Right
			MovedLocation + FVector(0,-CapsuleRadius,0), // Forward
			MovedLocation + FVector(0,CapsuleRadius,0)   // Back
		};

		// Perform multiple ground checks
		bool bHasGroundContact = false;
		FHitResult BestHit;
		float ClosestDistance = GroundTraceDistance;
		
		for (const FVector& CheckPoint : CheckPoints)
		{
			FHitResult GroundHit;
			FVector TraceStart = CheckPoint;
			FVector TraceEnd = CheckPoint + FVector::DownVector * GroundTraceDistance;
			
			if (GetWorld()->SweepSingleByChannel(
				GroundHit,
				TraceStart,
				TraceEnd,
				FQuat::Identity,
				ECC_Visibility,
				SweepShape,
				QueryParams
			))
			{
				if (GroundHit.bBlockingHit)
				{
					float Distance = (GroundHit.Location - CheckPoint).Size();
					if (!bHasGroundContact || Distance < ClosestDistance)
					{
						bHasGroundContact = true;
						ClosestDistance = Distance;
						BestHit = GroundHit;
					}
				}
			}
		}
		
		if (bHasGroundContact)
		{
			// Calculate slope angle using the hit normal
			float SlopeAngle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(BestHit.Normal, FVector::UpVector)));
			
			if (SlopeAngle <= MaxSlopeAngle)
			{
				FVector TargetLocation;
				
				// Handle steps and slopes differently
				if (SlopeAngle > StepDetectionAngle) // It's a slope
				{
					// Project velocity onto slope plane
					FVector ProjectedVelocity = FVector::VectorPlaneProject(Velocity, BestHit.Normal);
					Velocity = ProjectedVelocity;
					
					TargetLocation = BestHit.ImpactPoint;
				}
				else // It's potentially a step or flat ground
				{
					// Maintain horizontal velocity
					Velocity.Z = 0.0f;
					
					TargetLocation = BestHit.ImpactPoint;
				}
				
				// Use exponential smoothing for height adjustment
				FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
				FVector NewLocation = CurrentLocation;
				
				// Set Z position with capsule offset
				NewLocation.Z = TargetLocation.Z + CapsuleHalfHeight;
				
				UpdatedComponent->SetWorldLocation(NewLocation);
				
				// Apply adaptive ground stick force
				if (!bPositionCorrected)
				{
					float GroundDistance = FMath::Abs(CurrentLocation.Z - (TargetLocation.Z + CapsuleHalfHeight));
					float StickForce = FMath::GetMappedRangeValueClamped(
						FVector2D(0.0f, CapsuleRadius), // 使用胶囊体半径作为检测范围
						FVector2D(MinGroundStickForce, MaxGroundStickForce),
						GroundDistance
					);
					
					Velocity += FVector::DownVector * StickForce;
				}
			}
		}
	}

	// Finalize
	UpdateComponentVelocity();
}

bool UConsciousPawnMovementComponent::LimitWorldBounds()
{
	AWorldSettings* WorldSettings = PawnOwner ? PawnOwner->GetWorldSettings() : NULL;
	if (!WorldSettings || !WorldSettings->bEnableWorldBoundsChecks || !UpdatedComponent)
	{
		return false;
	}

	const FVector CurrentLocation = UpdatedComponent->GetComponentLocation();
	if (CurrentLocation.Z < WorldSettings->KillZ)
	{
		Velocity.Z = FMath::Min(GetMaxSpeed(), WorldSettings->KillZ - CurrentLocation.Z + 2.0f);
		return true;
	}

	return false;
}

void UConsciousPawnMovementComponent::ApplyControlInputToVelocity(float DeltaTime)
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

bool UConsciousPawnMovementComponent::ResolvePenetrationImpl(const FVector& Adjustment, const FHitResult& Hit,
                                                             const FQuat& NewRotationQuat)
{
	bPositionCorrected |= Super::ResolvePenetrationImpl(Adjustment, Hit, NewRotationQuat);
	return bPositionCorrected;
}
