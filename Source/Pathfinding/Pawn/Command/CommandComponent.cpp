// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "GameFramework/CharacterMovementComponent.h"

FTargetRequest::FTargetRequest(UCommandComponent* InCommand) : FTargetRequest()
{
	Command = InCommand;

	CommandName = Command ? InCommand->GetCommandName() : NAME_None;
}

UCommandComponent::UCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCommandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                      FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsExecuting())
	{
		InternalExecute(DeltaTime);
	}
}

FString UCommandComponent::GetCommandDescription_Implementation() const
{
	return Data.Description;
}

FString UCommandComponent::GetCommandDisplayName_Implementation() const
{
	return GetCommandName().ToString();
}

float UCommandComponent::GetRequiredTargetRadius_Implementation() const
{
	return Data.GetRequiredTargetRadius();
}

UObject* UCommandComponent::GetCommandIcon_Implementation() const
{
	return Data.Icon;
}

AConsciousPawn* UCommandComponent::GetExecutePawn() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

AConsciousAIController* UCommandComponent::GetExecuteController() const
{
	if (const AConsciousPawn* ExecutePawn = GetExecutePawn())
	{
		return Cast<AConsciousAIController>(ExecutePawn->GetController());
	}

	return nullptr;
}

void UCommandComponent::SetCommandArgumentsSkipCheck(const FTargetRequest& InRequest)
{
	Request = InRequest;
}

bool UCommandComponent::SetCommandArguments(const FTargetRequest& InRequest)
{
	SetCommandArgumentsSkipCheck(InRequest);

	return IsArgumentsValid();
}

bool UCommandComponent::IsArgumentsValid()
{
	return GetExecutePawn() != nullptr && InternalIsArgumentsValid();
}

bool UCommandComponent::IsTargetInRequiredRadius() const
{
	const float TargetRadius = GetRequiredTargetRadius();
	if (TargetRadius < 0)
	{
		return true;
	}

	const AConsciousPawn* ExecutePawn = GetExecutePawn();

	if (!Request.IsTargetPawnValid())
	{
		return FVector::Dist(Request.TargetLocation, ExecutePawn->GetActorLocation()) <= TargetRadius;
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
		FCollisionShape::MakeSphere(TargetRadius),
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
	return (!Data.bArgumentsValidCheckBeforeExecute || IsArgumentsValid())
		&& IsTargetInRequiredRadius()
		&& InternalCanExecute();
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

	// DEBUG_MESSAGE(
	// 	TEXT("Conscious Pawn [%s] Execute Command [%s]"),
	// 	*GetExecutePawn()->GetName(),
	// 	*GetCommandName().ToString()
	// );

	InternalBeginExecute();
}

void UCommandComponent::EndExecute(ECommandExecuteResult Result)
{
	if (!bExecuting)
	{
		return;
	}

	bExecuting = false;

	// DEBUG_MESSAGE(
	// 	TEXT("Conscious Pawn [%s] End Execute Command [%s]"),
	// 	*GetExecutePawn()->GetName(),
	// 	*GetCommandName().ToString()
	// );

	InternalEndExecute(Result);

	if (OnCommandEnd.IsBound())
	{
		OnCommandEnd.Broadcast(this, Result);
	}
}

void UCommandComponent::EndExecuteDelay(ECommandExecuteResult Result, float Duration)
{
	TSharedPtr<FTimerHandle> TimerHandle = MakeShared<FTimerHandle>();

	GetWorld()->GetTimerManager().SetTimer(
		*TimerHandle,
		FTimerDelegate::CreateLambda([this, Result, TimerHandle]()
		{
			GetWorld()->GetTimerManager().ClearTimer(*TimerHandle);
			EndExecute(Result);
		}),
		Duration,
		false
	);
}

void UCommandComponent::OnPushedToQueue()
{
	if (OnCommandPushedToQueue.IsBound())
	{
		OnCommandPushedToQueue.Broadcast(this);
	}

	// DEBUG_MESSAGE(
	// 	TEXT("Conscious Pawn [%s] Push Command [%s]"),
	// 	*GetExecutePawn()->GetName(),
	// 	*GetCommandName().ToString()
	// );

	InternalPushedToQueue();
}

void UCommandComponent::OnPoppedFromQueue(ECommandPoppedReason Reason)
{
	// DEBUG_MESSAGE(
	// 	TEXT("Conscious Pawn [%s] Pop Command [%s]"),
	// 	*GetExecutePawn()->GetName(),
	// 	*GetCommandName().ToString()
	// );

	InternalPoppedFromQueue(Reason);

	if (OnCommandPoppedFromQueue.IsBound())
	{
		OnCommandPoppedFromQueue.Broadcast(this, Reason);
	}
}

void UCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
}

void UCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
}

void UCommandComponent::InternalPushedToQueue_Implementation()
{
}

bool UCommandComponent::InternalIsArgumentsValid_Implementation()
{
	return true;
}

void UCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
}
