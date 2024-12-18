// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCommandComponent.h"

#include "ISourceControlProvider.h"
#include "PFUtils.h"
#include "Conscious/ConsciousPawn.h"
#include "Conscious/ConsciousAIController.h"

FName UMoveCommandComponent::CommandName = FName("Move");

UMoveCommandComponent::UMoveCommandComponent(): AcceptanceRadius(-1)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMoveCommandComponent::SetMoveCommandArgs(float InAcceptanceRadius)
{
	AcceptanceRadius = InAcceptanceRadius;
}

void UMoveCommandComponent::InternalBeginExecute_Implementation()
{
	AConsciousAIController* AIController = GetExecuteController();

	AIController->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveComplete);
	if (Request.TargetPawn)
	{
		AIController->MoveToActor(Request.TargetPawn, AcceptanceRadius);
	}
	else
	{
		AIController->MoveToLocation(Request.TargetLocation, AcceptanceRadius);
	}
}

void UMoveCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AConsciousAIController* AIController = GetExecuteController();

	if (Result == ECommandExecuteResult::Aborted)
	{
		AIController->StopMovement();
	}
}

void UMoveCommandComponent::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	AConsciousAIController* AIController = GetExecuteController();

	AIController->ReceiveMoveCompleted.RemoveDynamic(this, &ThisClass::OnMoveComplete);

	if (!IsExecuting())
	{
		return;
	}

	// FString ResultName;
	// switch (Result)
	// {
	// case EPathFollowingResult::Aborted:
	// 	ResultName = "Aborted";
	// 	break;
	// case EPathFollowingResult::Success:
	// 	ResultName = "Success";
	// 	break;
	// case EPathFollowingResult::Blocked:
	// 	ResultName = "Blocked";
	// 	break;
	// case EPathFollowingResult::Invalid:
	// 	ResultName = "Invalid";
	// 	break;
	// case EPathFollowingResult::OffPath:
	// 	ResultName = "OffPath";
	// 	break;
	// case EPathFollowingResult::Skipped_DEPRECATED:
	// 	ResultName = "Skipped_DEPRECATED";
	// 	break;
	// }
	//
	// DEBUG_MESSAGE(TEXT("%s"), *ResultName);

	ECommandExecuteResult ExecuteResult = ECommandExecuteResult::Success;

	if (Result != EPathFollowingResult::Success)
	{
		ExecuteResult = ECommandExecuteResult::Failed;
	}
	// else
	// {
	// 	float Distance;
	// 	const FVector CurrentLocation = GetExecutePawn()->GetActorLocation();
	// 	if (Request.TargetPawn)
	// 	{
	// 		Distance = FVector::Dist(CurrentLocation, Request.TargetPawn->GetActorLocation());
	// 	}
	// 	else
	// 	{
	// 		Distance = FVector::Dist(CurrentLocation, Request.TargetLocation);
	// 	}
	//
	// 	if (Distance > AcceptanceRadius)
	// 	{
	// 		ExecuteResult = ECommandExecuteResult::Failed; 
	// 	}
	// }
	
	EndExecute(ExecuteResult);
}
