// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"
#include "Command/MoveCommandComponent.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"


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
	UCommandComponent* RequestCommand = ResolveRequestCommand(Request);

	UMoveCommandComponent* MoveCommandComponent = GetMoveCommandComponent();
	// no move command
	if (MoveCommandComponent == nullptr)
	{
		if (RequestCommand && RequestCommand->GetRequiredTargetRadius() < 0 && RequestCommand->SetCommandArgs(Request))
		{
			OutCommandsToExecute.Add(MoveCommandComponent);
		}
	}
	// add move command if the request command need to move
	else
	{
		// ignore move command
		if (RequestCommand == MoveCommandComponent)
		{
			if (MoveCommandComponent->SetMoveCommandArgs(nullptr, Request))
			{
				OutCommandsToExecute.Add(MoveCommandComponent);;
			}
		}
		else
		{
			if (RequestCommand->SetCommandArgs(Request))
			{
				if (MoveCommandComponent->SetMoveCommandArgs(RequestCommand, Request))
				{
					OutCommandsToExecute.Add(MoveCommandComponent);
				}

				OutCommandsToExecute.Add(RequestCommand);
			}
		}
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestCommand(const FTargetRequest& Request)
{
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

	return RequestCommand;
}

UMoveCommandComponent* AConsciousPawn::GetMoveCommandComponent() const
{
	if (UCommandComponent* const* Command = Commands.Find(UMoveCommandComponent::CommandName))
	{
		return Cast<UMoveCommandComponent>(*Command);
	}
	return nullptr;
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
