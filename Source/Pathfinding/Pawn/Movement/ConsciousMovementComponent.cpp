#include "ConsciousMovementComponent.h"
#include "Net/UnrealNetwork.h"

UConsciousMovementComponent::UConsciousMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UConsciousMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UConsciousMovementComponent, CurrentVelocity);
	DOREPLIFETIME(UConsciousMovementComponent, RequestedVelocity);
}

void UConsciousMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		RequestedVelocity = bForceMaxSpeed ? MoveVelocity.GetSafeNormal() * MaxSpeed : MoveVelocity;
	}
}

void UConsciousMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!UpdatedComponent || !GetOwner())
	{
		return;
	}

	if (GetOwnerRole() == ROLE_SimulatedProxy)
	{
		SimulateMovement(DeltaTime);
	}
	else
	{
		PerformMovement(DeltaTime);
	}
}

void UConsciousMovementComponent::PerformMovement(float DeltaTime)
{
	if (!RequestedVelocity.IsZero())
	{
		// Apply acceleration
		CurrentVelocity = FMath::VInterpTo(
			CurrentVelocity,
			RequestedVelocity,
			DeltaTime,
			Acceleration / MaxSpeed
		);
	}
	else if (!CurrentVelocity.IsZero())
	{
		// Apply deceleration
		CurrentVelocity = FMath::VInterpTo(
			CurrentVelocity,
			FVector::ZeroVector,
			DeltaTime,
			Deceleration / MaxSpeed
		);
	}

	// Apply movement
	if (!CurrentVelocity.IsZero())
	{
		FVector Delta = CurrentVelocity * DeltaTime;
		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, UpdatedComponent->GetComponentRotation(), true, Hit);

		if (Hit.IsValidBlockingHit())
		{
			SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit);
		}

		// Save move for prediction
		if (GetOwnerRole() == ROLE_AutonomousProxy)
		{
			FConsciousMoveData NewMove;
			NewMove.TimeStamp = GetWorld()->GetTimeSeconds();
			NewMove.Location = UpdatedComponent->GetComponentLocation();
			NewMove.Velocity = CurrentVelocity;
			NewMove.MoveDirection = RequestedVelocity.GetSafeNormal();
			SaveMove(NewMove);
			
			ServerMove(NewMove);
		}
		else if (GetOwnerRole() == ROLE_Authority)
		{
			MulticastMove(UpdatedComponent->GetComponentLocation(), CurrentVelocity);
		}
	}
}

void UConsciousMovementComponent::SimulateMovement(float DeltaTime)
{
	FVector NewLocation = UpdatedComponent->GetComponentLocation() + CurrentVelocity * DeltaTime;
	UpdatedComponent->SetWorldLocation(NewLocation);
}

void UConsciousMovementComponent::ServerMove_Implementation(const FConsciousMoveData& MoveData)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		// Validate move
		float DeltaTime = GetWorld()->GetTimeSeconds() - LastServerTimeStamp;
		float MaxDistance = MaxSpeed * DeltaTime * 1.5f;  // Allow some tolerance
		
		if (FVector::Dist(UpdatedComponent->GetComponentLocation(), MoveData.Location) > MaxDistance)
		{
			// Move is invalid, correct client
			MulticastMove(UpdatedComponent->GetComponentLocation(), CurrentVelocity);
		}
		else
		{
			// Accept move
			UpdatedComponent->SetWorldLocation(MoveData.Location);
			CurrentVelocity = MoveData.Velocity;
			LastServerTimeStamp = MoveData.TimeStamp;
		}
	}
}

void UConsciousMovementComponent::MulticastMove_Implementation(const FVector& NewLocation, const FVector& NewVelocity)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		UpdatedComponent->SetWorldLocation(NewLocation);
		CurrentVelocity = NewVelocity;
	}
}

void UConsciousMovementComponent::SaveMove(const FConsciousMoveData& NewMove)
{
	SavedMoves.Add(NewMove);
	
	// Limit saved moves to prevent memory issues
	const int32 MaxSavedMoves = 96;
	if (SavedMoves.Num() > MaxSavedMoves)
	{
		SavedMoves.RemoveAt(0, SavedMoves.Num() - MaxSavedMoves);
	}
}

void UConsciousMovementComponent::ClearSavedMoves()
{
	SavedMoves.Empty();
}
