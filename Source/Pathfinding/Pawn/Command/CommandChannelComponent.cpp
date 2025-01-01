// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandChannelComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Net/UnrealNetwork.h"

UCommandChannelComponent::UCommandChannelComponent(): ChannelId(-1), CurrentCommand(nullptr)
{
	SetIsReplicatedByDefault(true);
}

UCommandChannelComponent* UCommandChannelComponent::NewCommandChannel(AConsciousPawn* Owner, int32 InChannelId)
{
	UActorComponent* NewComponent = Owner->AddComponentByClass(StaticClass(), false, FTransform::Identity, false);
	if (UCommandChannelComponent* CommandChannel = Cast<UCommandChannelComponent>(NewComponent))
	{
		CommandChannel->ChannelId = InChannelId;
		return CommandChannel;
	}

	return nullptr;
}

void UCommandChannelComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ChannelId);
	DOREPLIFETIME(ThisClass, CommandQueue);
	DOREPLIFETIME(ThisClass, CurrentCommand);
}

void UCommandChannelComponent::BeginPlay()
{
	Super::BeginPlay();

	// // DEBUG_MESSAGE(TEXT("Command channel [%d]"), GetChannelId());
	// if (AConsciousPawn* OwnerConsciousPawn = Cast<AConsciousPawn>(GetOwner()))
	// {
	// 	// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *OwnerConsciousPawn->GetName(),
	// 	//               GetChannelId());
	// 	OwnerConsciousPawn->AddCommandChannel(this);
	// }
}

UCommandComponent* UCommandChannelComponent::GetCurrentCommand() const
{
	return CurrentCommand;
}

const TArray<UCommandComponent*>& UCommandChannelComponent::GetCommandsInQueue() const
{
	static TArray<UCommandComponent*> Commands;
	Commands.Reset();

	for (const FCommandWrapper& CommandWrapper : CommandQueue)
	{
		Commands.Add(CommandWrapper.Command);
	}

	return Commands;
}

void UCommandChannelComponent::PushCommand(UCommandComponent* CommandToPush)
{
	if (CommandToPush)
	{
		CommandQueue.Push(CommandToPush);

		GetConsciousPawnOwner()->DispatchCommand_OnPushedToQueue(CommandToPush);
		OnRep_CommandQueue();
	}
}

void UCommandChannelComponent::PopCommand(int32 CommandIndexToPop)
{
	if (CommandIndexToPop == -1)
	{
		ExecuteNextCommand();
	}
	else if (CommandIndexToPop >= 0 && CommandIndexToPop < CommandQueue.Num())
	{
		// Remove the command from the queue
		UCommandComponent* CommandToPop = CommandQueue[CommandIndexToPop].Command;
		CommandQueue.RemoveAt(CommandIndexToPop);

		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandToPop, ECommandPoppedReason::Cancel);
		OnRep_CommandQueue();
	}
}

bool UCommandChannelComponent::BeginClear()
{
	return true;
	
	if (NumToClear == -1)
	{
		NumToClear = CommandQueue.Num();
		return true;
	}

	return false;
}

void UCommandChannelComponent::EndClear()
{
	NumToClear = -1;
}

void UCommandChannelComponent::ClearCommands(ECommandPoppedReason Reason)
{
	AbortCurrentCommand();

	// while (NumToClear > 0 && CommandQueue.Num() > 0)
	// {
	// 	const FCommandWrapper& CommandWrapper = CommandQueue[0];
	// 	CommandQueue.RemoveAt(0);
	// 	NumToClear--;
	//
	// 	GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandWrapper.Command, Reason);
	// }

	for (const FCommandWrapper& CommandWrapper : CommandQueue)
	{
		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandWrapper.Command, Reason);
	}

	CommandQueue.Reset();

	OnRep_CommandQueue();
}

void UCommandChannelComponent::ExecuteNextCommand()
{
	AbortCurrentCommand();

	if (CommandQueue.Num() == 0)
	{
		return;
	}

	UCommandComponent* NextCommand = CommandQueue[0].Get();
	CommandQueue.RemoveAt(0);

	if (NextCommand->CanExecute())
	{
		NextCommand->OnCommandEnd.AddDynamic(this, &UCommandChannelComponent::OnCommandEnd);
		CurrentCommand = NextCommand;
		OnRep_CurrentCommand();

		GetConsciousPawnOwner()->DispatchCommand_BeginExecute(NextCommand, NextCommand->GetRequest());
	}
	else
	{
		if (BeginClear())
		{
			GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(NextCommand, ECommandPoppedReason::Unreachable);
			ClearCommands(ECommandPoppedReason::PreTaskFailed);
			EndClear();
		}
	}
}

void UCommandChannelComponent::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		UCommandComponent* CommandToAbort = CurrentCommand;
		CurrentCommand = nullptr;
		
		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandToAbort, ECommandPoppedReason::Cancel);
		GetConsciousPawnOwner()->DispatchCommand_EndExecute(CommandToAbort, ECommandExecuteResult::Aborted);

		OnRep_CurrentCommand();
	}
}

AConsciousPawn* UCommandChannelComponent::GetConsciousPawnOwner() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

void UCommandChannelComponent::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	GetConsciousPawnOwner()->DispatchCommand_EndExecute(Command, Result);

	Command->OnCommandEnd.RemoveDynamic(this, &ThisClass::OnCommandEnd);
	CurrentCommand = nullptr;

	if (Result == ECommandExecuteResult::Success)
	{
		ExecuteNextCommand();
	}
	else if (Command->IsAbortCurrentCommand())
	{
		ECommandPoppedReason ClearReason;
		switch (Result)
		{
			case ECommandExecuteResult::Aborted:
				ClearReason = ECommandPoppedReason::Cancel;
				break;
			case ECommandExecuteResult::Failed:
				ClearReason = ECommandPoppedReason::PreTaskFailed;
				break;
			default:
				ClearReason = ECommandPoppedReason::Cancel;
				break;
		}
		
		CLEAR_COMMAND_CHANNEL(this, ClearReason);
	}
}

void UCommandChannelComponent::OnRep_ChannelId()
{
	// DEBUG_MESSAGE(TEXT("Command channel [%d]"), GetChannelId());
	if (AConsciousPawn* OwnerConsciousPawn = Cast<AConsciousPawn>(GetOwner()))
	{
		// DEBUG_MESSAGE(TEXT("ConsciousPawn [%s] add command channel [%d]"), *OwnerConsciousPawn->GetName(),
		//               GetChannelId());
		OwnerConsciousPawn->AddCommandChannel(this);
	}
}
