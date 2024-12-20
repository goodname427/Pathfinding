// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"

UCommandComponent::UCommandComponent(): Request()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FName UCommandComponent::GetCommandName_Implementation() const
{
	return NAME_None;
}

float UCommandComponent::GetRequiredTargetRadius_Implementation() const
{
	return 250.0f;
}

AConsciousPawn* UCommandComponent::GetExecutePawn() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

AConsciousAIController* UCommandComponent::GetExecuteController() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return ExecutePawn->GetConsciousAIController();
	}

	return nullptr;
}

bool UCommandComponent::SetCommandArgs(const FTargetRequest& InRequest)
{
	Request = InRequest;

	return IsReachable();
}

bool UCommandComponent::IsReachable()
{
	return GetExecutePawn() != nullptr && InternalIsReachable();
}

bool UCommandComponent::IsTargetReachable() const
{
	const float RequiredTargetRadius = GetRequiredTargetRadius();
	if (RequiredTargetRadius < 0)
	{
		return true;
	}

	const AConsciousPawn* ExecutePawn = GetExecutePawn();

	if (!Request.TargetPawn)
	{
		return FVector::Dist(Request.TargetLocation, ExecutePawn->GetActorLocation()) <= RequiredTargetRadius;
	}

	const FVector CurrentLocation = ExecutePawn->GetActorLocation();

	TArray<FHitResult> HitResults;
	// FCollisionQueryParams Params;
	// Params.AddIgnoredActor(ExecutePawn);

	GetWorld()->SweepMultiByChannel(
		HitResults,
		CurrentLocation,
		CurrentLocation,
		FQuat::Identity,
		ECC_Camera,
		FCollisionShape::MakeSphere(RequiredTargetRadius),
		FCollisionQueryParams::DefaultQueryParam
	);

	for (const FHitResult& Hit : HitResults)
	{
		if (Hit.Actor == Request.TargetPawn)
		{
			return true;
		}
	}

	return false;
}

void UCommandComponent::InternalBeginExecute_Implementation()
{
}

bool UCommandComponent::InternalCanExecute_Implementation()
{
	return true;
}

bool UCommandComponent::CanExecute()
{
	return IsTargetReachable() && InternalCanExecute();
}

void UCommandComponent::BeginExecute()
{
	if (bExecuting)
	{
		return;
	}

	bExecuting = true;

	if (OnCommandBegin.IsBound())
	{
		OnCommandBegin.Broadcast(this);
	}

	DEBUG_MESSAGE(TEXT("Conscious Pawn [%s] Execute Command [%s]"), *GetExecutePawn()->GetName(),
	              *GetCommandName().ToString());

	InternalBeginExecute();
}

void UCommandComponent::EndExecute(ECommandExecuteResult Result)
{
	if (!bExecuting)
	{
		return;
	}

	bExecuting = false;

	InternalEndExecute(Result);

	if (OnCommandEnd.IsBound())
	{
		OnCommandEnd.Broadcast(this, Result);
	}
}

bool UCommandComponent::InternalIsReachable_Implementation()
{
	return true;
}

void UCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
}
