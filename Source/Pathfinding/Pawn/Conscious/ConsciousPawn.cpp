// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"

#include "Command/MoveCommandComponent.h"


// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousAIController(nullptr)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	

	// RootComponent->SetSimulatePhysics();
	// StaticMeshComponent->SetSimulatePhysics(true);
	// StaticMeshComponent->SetConstraintMode(EDOFMode::XYPlane);
	// StaticMeshComponent->BodyInstance.bLockXRotation = true;
	// StaticMeshComponent->BodyInstance.bLockYRotation = true;
	// StaticMeshComponent->bApplyImpulseOnDamage = false;
}

void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();

	for (UActorComponent* Component : GetComponents())
	{
		UCommandComponent* Command = Cast<UCommandComponent>(Component);
		if (Command)
		{
			Commands.Add(Command->GetCommandName(), Command);
		}
	}
}

void AConsciousPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ConsciousAIController = Cast<AConsciousAIController>(NewController);
}

void AConsciousPawn::Receive(const FTargetRequest& Request, bool bStartNewCommandQueue)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	if (!ConsciousAIController)
	{
		// DEBUG_MESSAGE(TEXT("ConsciousAIController Is Not Ready!"));
		return;
	}

	OnReceive(Request, bStartNewCommandQueue);

	if (bStartNewCommandQueue)
	{
		ConsciousAIController->ClearAllCommands();
	}

	static TArray<UCommandComponent*> CommandsToExecute;
	ResolveRequest(CommandsToExecute, Request);

	for (UCommandComponent* CommandToExecute : CommandsToExecute)
	{
		ConsciousAIController->PushCommand(CommandToExecute);
	}

	if (bStartNewCommandQueue)
	{
		ConsciousAIController->ExecuteCommandQueue();
	}
}

void AConsciousPawn::OnReceive_Implementation(const FTargetRequest& Request, bool bStartNewCommandQueue)
{
}

void AConsciousPawn::ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request)
{
	OutCommandsToExecute.Reset();
	UCommandComponent* RequestCommand = nullptr;
	if (Request.CommandName != NAME_None)
	{
		if (UCommandComponent** RequestCommandPtr = Commands.Find(Request.CommandName))
		{
			RequestCommand = *RequestCommandPtr;
		}
	}
	else
	{
		RequestCommand = ResolveRequestWithoutName(Request);
	}


	if (RequestCommand)
	{
		if (RequestCommand->GetCommandName() != UMoveCommandComponent::CommandName)
		{
			RequestCommand->SetCommandArgs(Request);
			if (!RequestCommand->IsReachable())
			{
				return;
			}
			
			float RequiredTargetRadius = RequestCommand->GetRequiredTargetRadius();
			if (RequiredTargetRadius >= 0)
			{
				UMoveCommandComponent* MoveCommand = GetMoveCommandComponent();

				if (MoveCommand)
				{
					MoveCommand->SetCommandArgs(Request);
					MoveCommand->SetMoveCommandArgs(RequestCommand);
					if (!MoveCommand->IsReachable())
					{
						return;
					}
					
					OutCommandsToExecute.Add(MoveCommand);
				}
			}
		}
		else
		{
			// Clear Move Command
			UMoveCommandComponent* MoveCommand = Cast<UMoveCommandComponent>(RequestCommand);

			if (MoveCommand)
			{
				MoveCommand->SetCommandArgs(Request);
				MoveCommand->SetMoveCommandArgs(nullptr);
				if (!MoveCommand->IsReachable())
				{
					return;
				}
			}
		}

		OutCommandsToExecute.Add(RequestCommand);
	}
}

UMoveCommandComponent* AConsciousPawn::GetMoveCommandComponent() const
{
	if (UCommandComponent* const* MoveCommandPtr = Commands.Find(UMoveCommandComponent::CommandName))
	{
		return Cast<UMoveCommandComponent>(*MoveCommandPtr);
	}

	return nullptr;
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
