// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "Controller/ConsciousAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PFUtils.h"
#include "Command/CommandChannelComponent.h"
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
		UCommandChannelComponent* RequestCommandChannel = GetCommandChannel(Request.OverrideCommandChannel);

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
				CLEAR_COMMAND_CHANNEL(RequestCommandChannel, ECommandPoppedReason::Cancel);
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
	UCommandChannelComponent* CommandChannel = GetOrCreateCommandChannel(GET_COMMAND_CHANNEL(Request, FirstCommand));

	const bool bAbortCurrentCommand = Request.Type == ETargetRequestType::StartNew && FirstCommand->
		IsAbortCurrentCommand();

	if (bAbortCurrentCommand)
	{
		CLEAR_COMMAND_CHANNEL(CommandChannel, ECommandPoppedReason::Cancel);
	}

	for (UCommandComponent* CommandToExecute : CommandsToExecute)
	{
		// DEBUG_MESSAGE(TEXT("ConsciousAIController Push Command [%s]"), *CommandToExecute->GetCommandName().ToString());
		CommandChannel->PushCommand(CommandToExecute);
	}

	if (bAbortCurrentCommand || CommandChannel->GetCurrentCommand() == nullptr)
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

	// no command
	if (RequestCommand == nullptr)
	{
		return;
	}

	UMoveCommandComponent* MoveCommandComponent = GetMoveCommandComponent();

	if (RequestCommand == MoveCommandComponent)
	{
		if (MoveCommandComponent->SetMoveCommandArguments(nullptr, Request))
		{
			OutCommandsToExecute.Add(MoveCommandComponent);;
		}
	}
	else
	{
		// needn't to move
		if (RequestCommand->GetRequiredTargetRadius() < 0)
		{
			if (RequestCommand->SetCommandArguments(Request))
			{
				OutCommandsToExecute.Add(RequestCommand);
			}
		}
		// add move command if the request command need to move
		else if (MoveCommandComponent != nullptr)
		{
			if (RequestCommand->SetCommandArguments(Request))
			{
				if (MoveCommandComponent->SetMoveCommandArguments(RequestCommand, Request))
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

void AConsciousPawn::DispatchCommand_OnPushedToQueue_Implementation(UCommandComponent* Command)
{
	if (!Command)
		return;

	Command->OnPushedToQueue();
}

void AConsciousPawn::DispatchCommand_EndExecute_Implementation(UCommandComponent* Command, ECommandExecuteResult Result)
{
	if (!Command)
		return;

	Command->EndExecute(Result);
}

void AConsciousPawn::DispatchCommand_BeginExecute_Implementation(UCommandComponent* Command,
                                                                 const FTargetRequest& Request)
{
	if (!Command)
	{
		// DEBUG_MESSAGE(TEXT("Command is null"));	
		return;
	}

	Command->SetCommandArguments(Request);
	Command->BeginExecute();
}

void AConsciousPawn::DispatchCommand_OnPoppedFromQueue_Implementation(UCommandComponent* Command,
                                                                      ECommandPoppedReason Reason)
{
	if (!Command)
		return;

	Command->OnPoppedFromQueue(Reason);
}

UCommandComponent* AConsciousPawn::GetCurrentCommand(int32 ChannelId) const
{
	if (UCommandChannelComponent* CommandChannel = GetCommandChannel(ChannelId))
	{
		return CommandChannel->GetCurrentCommand();
	}

	return nullptr;
}

const TArray<UCommandComponent*>& AConsciousPawn::GetCommandsInQueue(int32 ChannelId) const
{
	if (UCommandChannelComponent* CommandChannel = GetCommandChannel(ChannelId))
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

UCommandChannelComponent* AConsciousPawn::GetCommandChannel(int32 ChannelId) const
{
	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannelComponent* const* CommandChannel = CommandChannelMap.Find(ChannelId))
	{
		return *CommandChannel;
	}

	return nullptr;
}

UCommandChannelComponent* AConsciousPawn::GetOrCreateCommandChannel(int32 ChannelId)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return nullptr;
	}

	if (ChannelId < GCommandChannel_Default)
	{
		return nullptr;
	}

	if (UCommandChannelComponent*& FoundCommandChannel = CommandChannelMap.FindOrAdd(ChannelId))
	{
		return FoundCommandChannel;
	}
	else
	{
		FoundCommandChannel = UCommandChannelComponent::NewCommandChannel(this, ChannelId);
		// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *GetName(), FoundCommandChannel->GetChannelId());

		if (OnCommandChannelCreated.IsBound())
		{
			OnCommandChannelCreated.Broadcast(this);
		}

		return FoundCommandChannel;
	}
}

void AConsciousPawn::AddCommandChannel(UCommandChannelComponent* CommandChannel)
{
	if (GetLocalRole() >= ROLE_Authority)
	{
		return;
	}

	if (!CommandChannelMap.Contains(CommandChannel->GetChannelId()))
	{
		// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *GetName(), CommandChannel->GetChannelId());
		CommandChannelMap.Add(CommandChannel->GetChannelId(), CommandChannel);
		if (OnCommandChannelCreated.IsBound())
		{
			OnCommandChannelCreated.Broadcast(this);
		}
	}
}

UCommandComponent* AConsciousPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return nullptr;
}
