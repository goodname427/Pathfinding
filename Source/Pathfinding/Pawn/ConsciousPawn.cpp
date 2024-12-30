// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"
#include "Command/MoveCommandComponent.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"


// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousAIController(nullptr), ConsciousData()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();

	for (UActorComponent* Component : GetComponents())
	{
		UCommandComponent* Command = Cast<UCommandComponent>(Component);
		if (Command)
		{
			if (Command->IsA<UProgressCommandComponent>())
			{
				bHasProgressCommand = true;
			}
			
			Commands.Add(Command->GetCommandName(), Command);
		}
	}
}

void AConsciousPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ConsciousAIController = Cast<AConsciousAIController>(NewController);
}

void AConsciousPawn::Receive(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	if (!ConsciousAIController)
	{
		// DEBUG_MESSAGE(TEXT("ConsciousAIController Is Not Ready!"));
		return;
	}

	OnReceive(Request);

	if (Request.Type == ETargetRequestType::Clear)
	{
		ConsciousAIController->ClearCommands();
		return;
	}

	if (Request.Type == ETargetRequestType::AbortCurrent)
	{
		ConsciousAIController->AbortCurrentCommand();
		return;
	}

	if (Request.Type == ETargetRequestType::Pop)
	{
		ConsciousAIController->PopCommand(Request);
		return;
	}

	// Append and StartNew
	static TArray<UCommandComponent*> CommandsToExecute;
	ResolveRequest(CommandsToExecute, Request);

	const UCommandComponent* FirstCommand = nullptr;
	if (CommandsToExecute.Num() > 0)
	{
		FirstCommand = CommandsToExecute[0];
		
		if (Request.Type == ETargetRequestType::StartNew && FirstCommand->IsAbortCurrentCommand())
		{
			ConsciousAIController->ClearCommands();
		}
	}

	for (UCommandComponent* CommandToExecute : CommandsToExecute)
	{
		// DEBUG_MESSAGE(TEXT("ConsciousAIController Push Command [%s]"), *CommandToExecute->GetCommandName().ToString());
		ConsciousAIController->PushCommand(CommandToExecute);
	}

	if (Request.Type == ETargetRequestType::StartNew
		&& FirstCommand
		&& (FirstCommand->IsAbortCurrentCommand() || !ConsciousAIController->GetCurrentCommand()))
	{
		ConsciousAIController->ExecuteNextCommand();
	}
}

void AConsciousPawn::OnReceive_Implementation(const FTargetRequest& Request)
{
}

void AConsciousPawn::ResolveRequest(TArray<UCommandComponent*>& OutCommandsToExecute, const FTargetRequest& Request)
{
	OutCommandsToExecute.Reset();
	UCommandComponent* RequestCommand = ResolveRequestCommand(Request);

	if (RequestCommand == nullptr)
	{
		return;
	}
		
	UMoveCommandComponent* MoveCommandComponent = GetMoveCommandComponent();
	// no move command
	if (MoveCommandComponent == nullptr)
	{
		if (RequestCommand && RequestCommand->GetRequiredTargetRadius() < 0 && RequestCommand->SetCommandArgs(Request))
		{
			OutCommandsToExecute.Add(RequestCommand);
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
	if (Request.Command != nullptr && Request.Command->GetExecutePawn() == this)
	{
		RequestCommand = Request.Command;
	}
	else if (Request.CommandName != NAME_None)
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
	return GetCommandByName<UMoveCommandComponent>();
}

UCommandComponent* AConsciousPawn::GetCommandByName(FName CommandName) const
{
	if (UCommandComponent* const* FoundCommand = Commands.Find(CommandName))
	{
		return *FoundCommand;
	}
	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsByName(FName CommandName) const
{
	static TArray<UCommandComponent*> FoundCommands;
	FoundCommands.Reset();

	Commands.MultiFind(CommandName, FoundCommands);

	return FoundCommands;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetAllCommands() const
{
	static TArray<UCommandComponent*> CommandArray;

	Commands.GenerateValueArray(CommandArray);

	return CommandArray;
}

const UCommandComponent* AConsciousPawn::AddCommand(TSubclassOf<UCommandComponent> CommandClassToAdd)
{
	UCommandComponent* NewCommand = Cast<UCommandComponent>(AddComponentByClass(CommandClassToAdd, false, FTransform::Identity, true));
	if (NewCommand)
	{
		if (NewCommand->IsA<UProgressCommandComponent>())
		{
			bHasProgressCommand = true;
		}
		
		Commands.Add(NewCommand->GetCommandName(), NewCommand);
	}

	return NewCommand;
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
