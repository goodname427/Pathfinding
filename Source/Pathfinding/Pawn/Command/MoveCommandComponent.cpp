// Fill out your copyright notice in the Description page of Project Settings.


#include "MoveCommandComponent.h"

#include "CommanderPawn.h"
#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Controller/ConsciousAIController.h"

FName UMoveCommandComponent::StaticCommandName = FName("Move");

UMoveCommandComponent::UMoveCommandComponent(): CommandNeedToMove(nullptr), FlagActor(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
}

bool UMoveCommandComponent::SetMoveCommandArguments(UCommandComponent* InCommandNeedToMove,
                                                    const FTargetRequest& InRequest)
{
	CommandNeedToMove = InCommandNeedToMove;

	if (CommandNeedToMove == nullptr)
	{
		return SetCommandArguments(InRequest);
	}

	// Move command reachable check
	return CommandNeedToMove->GetRequiredTargetRadius() >= 0 && SetCommandArguments(InRequest);
}

void UMoveCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();

	AConsciousAIController* AIController = GetExecuteController();

	AIController->ReceiveMoveCompleted.AddDynamic(this, &ThisClass::OnMoveComplete);
	
	if (Request.IsTargetPawnValid())
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
	AIController->StopMovement();
}

void UMoveCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	AUTHORITY_CHECK();
	
	if (CommandNeedToMove)
	{
		if (CommandNeedToMove->IsTargetInRequiredRadius())
		{
			EndExecute(ECommandExecuteResult::Success);
			return;
		}
		
		if (!CommandNeedToMove->IsCommandEnable(true)
			|| !CommandNeedToMove->IsArgumentsValid(true))
		{
			// DEBUG_FUNC_FLAG();
			EndExecute(ECommandExecuteResult::Failed);
		}
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

	// DEBUG_FUNC_FLAG();
	EndExecute(ExecuteResult);
}

void UMoveCommandComponent::InternalBeginTarget_Implementation()
{
	FlagActor = GetWorld()->SpawnActor(FlagActorClass);
}

void UMoveCommandComponent::InternalEndTarget_Implementation()
{
	if (FlagActor)
	{
		FlagActor->Destroy();
	}
}

void UMoveCommandComponent::InternalTarget_Implementation(float DeltaTime)
{
	if (FlagActor)
	{
		const FRay Ray = TargetCommander->GetRayFromMousePosition();

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Ray.Origin, Ray.PointAt(100 * 100), ECC_Visibility);

		if(Hit.bBlockingHit)
		{
			FlagActor->SetActorLocation(Hit.Location);
		}
	}
}
