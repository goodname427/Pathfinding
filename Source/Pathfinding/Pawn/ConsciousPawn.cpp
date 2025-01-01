// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"
#include "Command/CommandChannel.h"
#include "Command/MoveCommandComponent.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"
#include "Net/UnrealNetwork.h"


// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousData()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AConsciousPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AConsciousPawn::PostInitProperties()
{
	Super::PostInitProperties();
}

void AConsciousPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	for (UActorComponent* Component : GetComponents())
	{
		UCommandComponent* Command = Cast<UCommandComponent>(Component);
		if (Command)
		{
			CommandList.Add(Command->GetCommandName(), Command);
		}
	}
}

void AConsciousPawn::Receive_Implementation(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	OnReceive(Request);

	if (Request.Type == ETargetRequestType::AbortOrPop
		|| Request.Type == ETargetRequestType::Clear)
	{
		ACommandChannel* RequestCommandChannel = GetCommandChannel(Request.OverrideCommandChannel);

		if (!RequestCommandChannel)
		{
			return;
		}

		switch (Request.Type)
		{
		case ETargetRequestType::AbortOrPop:
			{
				RequestCommandChannel->PopCommand(Request.CommandIndexToPop);
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
	ACommandChannel* CommandChannel = GetOrCreateCommandChannel(GET_COMMAND_CHANNEL(Request, FirstCommand));

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
		if (UCommandComponent** RequestCommandPtr = CommandList.Find(Request.CommandName))
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
	if (UCommandComponent* const* FoundCommand = CommandList.Find(CommandName))
	{
		return *FoundCommand;
	}
	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsByName(FName CommandName) const
{
	static TArray<UCommandComponent*> FoundCommands;
	FoundCommands.Reset();

	CommandList.MultiFind(CommandName, FoundCommands);

	return FoundCommands;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetAllCommands() const
{
	static TArray<UCommandComponent*> CommandArray;

	CommandList.GenerateValueArray(CommandArray);

	return CommandArray;
}

const UCommandComponent* AConsciousPawn::AddCommand(TSubclassOf<UCommandComponent> CommandClassToAdd)
{
	UCommandComponent* NewCommand = Cast<UCommandComponent>(
		AddComponentByClass(CommandClassToAdd, false, FTransform::Identity, true));
	if (NewCommand)
	{
		CommandList.Add(NewCommand->GetCommandName(), NewCommand);
	}

	return NewCommand;
}

UCommandComponent* AConsciousPawn::GetCurrentCommand(int32 ChannelId) const
{
	if (ACommandChannel* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCurrentCommand();
	}

	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsInQueue(int32 ChannelId) const
{
	if (ACommandChannel* CommandChannel = GetCommandChannel(ChannelId))
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

ACommandChannel* AConsciousPawn::GetCommandChannel(int32 ChannelId) const
{
	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (ACommandChannel* const* CommandChannel = CommandChannelMap.Find(ChannelId))
	{
		return *CommandChannel;
	}

	return nullptr;
}

ACommandChannel* AConsciousPawn::GetOrCreateCommandChannel(int32 ChannelId)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return nullptr;
	}

	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (ACommandChannel*& FoundCommandChannel = CommandChannelMap.FindOrAdd(ChannelId))
	{
		return FoundCommandChannel;
	}
	else
	{
		FoundCommandChannel = ACommandChannel::NewCommandChannel(this, ChannelId);

		if (OnCommandChannelCreated.IsBound())
		{
			OnCommandChannelCreated.Broadcast(this);
		}

		return FoundCommandChannel;
	}
}

void AConsciousPawn::AddCommandChannel(ACommandChannel* CommandChannel)
{
	if (GetLocalRole() >= ROLE_Authority)
	{
		return;
	}

	// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *GetName(), CommandChannel->GetChannelId());
	CommandChannelMap.Add(CommandChannel->GetChannelId(), CommandChannel);
	if (OnCommandChannelCreated.IsBound())
	{
		OnCommandChannelCreated.Broadcast(this);
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
