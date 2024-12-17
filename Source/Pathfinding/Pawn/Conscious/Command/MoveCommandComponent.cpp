// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCommandComponent.h"

#include "ISourceControlProvider.h"
#include "PFUtils.h"
#include "Conscious/ConsciousPawn.h"
#include "Conscious/ConsciousAIController.h"

FName UMoveCommandComponent::MoveCommandName = FName("Move");

void UMoveCommandComponent::SetMoveCommandArgs(float InAcceptanceRadius)
{
	AcceptanceRadius = InAcceptanceRadius;
}

void UMoveCommandComponent::InternalBeginExecute_Implementation()
{
	AConsciousPawn* ConsciousPawn = GetExecutePawn();
	AConsciousAIController* AIController = ConsciousPawn->GetConsciousAIController();

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
	AConsciousPawn* ConsciousPawn = GetExecutePawn();
	AConsciousAIController* AIController = ConsciousPawn->GetConsciousAIController();

	if (Result == ECommandExecuteResult::Aborted)
	{
		AIController->StopMovement();
	}
}

void UMoveCommandComponent::OnMoveComplete(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	AConsciousPawn* ConsciousPawn = GetExecutePawn();
	AConsciousAIController* AIController = ConsciousPawn->GetConsciousAIController();

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
	
	EndExecute(Result == EPathFollowingResult::Success ? ECommandExecuteResult::Success : ECommandExecuteResult::Failed);
}
