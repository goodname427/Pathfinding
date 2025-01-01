// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Controller/ConsciousAIController.h"

FName UMoveCommandComponent::StaticCommandName = FName("Move");

UMoveCommandComponent::UMoveCommandComponent(): CommandNeedToMove(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
}

void UMoveCommandComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                          FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// if (IsExecuting() && CommandNeedToMove && CommandNeedToMove->IsTargetInRequiredRadius())
	// {
	// 	GetExecuteController()->StopMovement();
	// 	EndExecute(ECommandExecuteResult::Success);
	// }
}

bool UMoveCommandComponent::SetMoveCommandArgs(UCommandComponent* InCommandNeedToMove, const FTargetRequest& InRequest)
{
	CommandNeedToMove = InCommandNeedToMove;

	if (CommandNeedToMove == nullptr)
	{
		return SetCommandArgs(InRequest);
	}

	// Move command reachable check
	return CommandNeedToMove->GetRequiredTargetRadius() >= 0 && SetCommandArgs(InRequest);
}

void UMoveCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();
	
	AConsciousAIController* AIController = GetExecuteController();

	AIController->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveComplete);
	if (Request.TargetPawn)
	{
		AIController->MoveToActor(Request.TargetPawn, -1, false);
	}
	else
	{
		AIController->MoveToLocation(Request.TargetLocation);
	}
}

void UMoveCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AUTHORITY_CHECK();
	
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

	ECommandExecuteResult ExecuteResult = ECommandExecuteResult::Success;

	if (Result != EPathFollowingResult::Success)
	{
		ExecuteResult = ECommandExecuteResult::Failed;
	}

	EndExecute(ExecuteResult);
}
