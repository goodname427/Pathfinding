// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"
#include "Command/CommandChannel.h"
#include "Command/MoveCommandComponent.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"


// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousData()
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
			Commands.Add(Command->GetCommandName(), Command);
		}
	}
}

void AConsciousPawn::Receive(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	OnReceive(Request);

	if (Request.Type == ETargetRequestType::AbortCurrent
		|| Request.Type == ETargetRequestType::Pop
		|| Request.Type == ETargetRequestType::Clear)
	{
		UCommandChannel* RequestCommandChannel = GetCommandChannel(Request.OverrideCommandChannel);

		if (!RequestCommandChannel)
		{
			return;
		}

		switch (Request.Type)
		{
		case ETargetRequestType::AbortCurrent:
			{
				RequestCommandChannel->ExecuteNextCommand();
				break;
			}
		case ETargetRequestType::Pop:
			{
				RequestCommandChannel->PopCommand(Request);
				break;
			}
		case ETargetRequestType::Clear:
			{
				RequestCommandChannel->ClearCommands();
				break;
			}
		default:
			{
				break;
			}
		}

		return;
	}
	
	// Append and StartNew
	static TArray<UCommandComponent*> CommandsToExecute;
	ResolveRequest(CommandsToExecute, Request);

	if (CommandsToExecute.Num() == 0)
	{
		return;
	}

	const UCommandComponent* FirstCommand = CommandsToExecute[0];
	UCommandChannel* CommandChannel = GetOrCreateCommandChannel(GET_COMMAND_CHANNEL(Request, FirstCommand));

	if (Request.Type == ETargetRequestType::StartNew && FirstCommand->IsAbortCurrentCommand())
	{
		CommandChannel->ClearCommands();
	}

	for (UCommandComponent* CommandToExecute : CommandsToExecute)
	{
		// DEBUG_MESSAGE(TEXT("ConsciousAIController Push Command [%s]"), *CommandToExecute->GetCommandName().ToString());
		CommandChannel->PushCommand(CommandToExecute);
	}

	if (Request.Type == ETargetRequestType::StartNew
		&& FirstCommand
		&& (FirstCommand->IsAbortCurrentCommand() || !CommandChannel->GetCurrentCommand()))
	{
		CommandChannel->ExecuteNextCommand();
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
	UCommandComponent* NewCommand = Cast<UCommandComponent>(
		AddComponentByClass(CommandClassToAdd, false, FTransform::Identity, true));
	if (NewCommand)
	{
		Commands.Add(NewCommand->GetCommandName(), NewCommand);
	}

	return NewCommand;
}

UCommandComponent* AConsciousPawn::GetCurrentCommand(int32 ChannelId) const
{
	if (UCommandChannel* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCurrentCommand();
	}

	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsInQueue(int32 ChannelId) const
{
	if (UCommandChannel* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCommandsInQueue();
	}

	static TArray<UCommandComponent*> EmptyCommands;
	return EmptyCommands;
}

const TArray<UProgressCommandComponent*>& AConsciousPawn::GetProgressCommandsInQueue() const
{
	static TArray<UProgressCommandComponent*> ProgressCommands;
	ProgressCommands.Reset();

	for (UCommandComponent* Command : GetCommandsInQueue(UProgressCommandComponent::StaticCommandChannel))
	{
		if (UProgressCommandComponent* ProgressCommand = Cast<UProgressCommandComponent>(Command))
		{
			ProgressCommands.Add(ProgressCommand);
		}
	}

	return ProgressCommands;
}

UCommandChannel* AConsciousPawn::GetCommandChannel(int32 ChannelId) const
{
	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannel* const* CommandChannel = CommandChannels.Find(ChannelId))
	{
		return *CommandChannel;
	}

	return nullptr;
}

UCommandChannel* AConsciousPawn::GetOrCreateCommandChannel(int32 ChannelId)
{
	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannel*& FoundCommandChannel = CommandChannels.FindOrAdd(ChannelId))
	{
		return FoundCommandChannel;
	}
	else
	{
		FoundCommandChannel = NewObject<UCommandChannel>();
		if (OnNewCommandChannelCreated.IsBound())
		{
			OnNewCommandChannelCreated.Broadcast(this, FoundCommandChannel);
		}
		return FoundCommandChannel;
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
