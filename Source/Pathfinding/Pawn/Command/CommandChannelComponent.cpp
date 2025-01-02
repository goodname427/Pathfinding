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
		if (bIsClearing)
		{
			PendingCommandQueue.Push(CommandToPush);
		}
		else
		{
			CommandQueue.Push(CommandToPush);
		}

		DispatchCommand(OnPushedToQueue, CommandToPush);
		OnRep_CommandQueue();
	}
}

void UCommandChannelComponent::PopCommand(int32 CommandIndexToPop)
{
	if (CommandIndexToPop == -1)
	{
		AbortCurrentCommand();
	}
	else if (CommandIndexToPop >= 0 && CommandIndexToPop < CommandQueue.Num())
	{
		// Remove the command from the queue
		UCommandComponent* CommandToPop = CommandQueue[CommandIndexToPop].Command;
		CommandQueue.RemoveAt(CommandIndexToPop);

		DispatchCommand(OnPoppedFromQueue, CommandToPop, ECommandPoppedReason::Cancel);
		OnRep_CommandQueue();
	}
}

bool UCommandChannelComponent::BeginClear()
{
	// return true;
	if (bIsClearing)
	{
		return false;
	}

	bIsClearing = true;
	//NumToClear = CommandQueue.Num();
	PendingCommandQueue.Empty();

	return true;
}

void UCommandChannelComponent::EndClear()
{
	bIsClearing = false;

	//NumToClear = -1;
	CommandQueue = MoveTemp(PendingCommandQueue);
	ExecuteNextCommand();
}

void UCommandChannelComponent::ClearCommands(ECommandPoppedReason Reason)
{
	AbortCurrentCommand();

	while (CommandQueue.Num() > 0)
	{
		UCommandComponent* PoppedCommand = CommandQueue[0].Command;
		CommandQueue.RemoveAt(0);
		//NumToClear--;

		DispatchCommand(OnPoppedFromQueue, PoppedCommand, Reason);
	}

	// for (const FCommandWrapper& CommandWrapper : CommandQueue)
	// {
	// 	GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandWrapper.Command, Reason);
	// }
	// CommandQueue.Reset();

	OnRep_CommandQueue();
}

void UCommandChannelComponent::ExecuteNextCommand()
{
	// wait for the clean step to finish
	if (bIsClearing)
	{
		return;
	}

	// abort current command will execute the next command, so don't need to execute subsequent process
	if (AbortCurrentCommand())
	{
		return;
	}

	if (CommandQueue.Num() == 0)
	{
		return;
	}

	// get the next command
	UCommandComponent* NextCommand = CommandQueue[0].Get();
	CommandQueue.RemoveAt(0);

	// check if the command can be executed
	if (NextCommand->CanExecute())
	{
		NextCommand->OnCommandEnd.AddDynamic(this, &UCommandChannelComponent::OnCommandEnd);
		CurrentCommand = NextCommand;

		DispatchCommand(BeginExecute, NextCommand, NextCommand->GetRequest());

		OnRep_CurrentCommand();
	}
	// cannot execute the command
	else
	{
		if (BeginClear())
		{
			DispatchCommand(OnPoppedFromQueue, NextCommand, ECommandPoppedReason::CanNotExecute);
			ClearCommands(ECommandPoppedReason::PreTaskFailed);
			EndClear();
		}
	}
}

bool UCommandChannelComponent::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		DispatchCommand(OnPoppedFromQueue, CurrentCommand, ECommandPoppedReason::Cancel);
		DispatchCommand(EndExecute, CurrentCommand, ECommandExecuteResult::Aborted);

		OnRep_CurrentCommand();
		return true;
	}

	return false;
}

AConsciousPawn* UCommandChannelComponent::GetConsciousPawnOwner() const
{
	return Cast<AConsciousPawn>(GetOwner());
}

void UCommandChannelComponent::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	Command->OnCommandEnd.RemoveDynamic(this, &ThisClass::OnCommandEnd);
	CurrentCommand = nullptr;

	DispatchCommand(EndExecute, Command, Result);

	switch (Result)
	{
	case ECommandExecuteResult::Success:
		{
			ExecuteNextCommand();
			break;
		}
	case ECommandExecuteResult::Failed:
		{
			CLEAR_COMMAND_CHANNEL(this, ECommandPoppedReason::PreTaskFailed);
			break;
		}
	case ECommandExecuteResult::Aborted:
		{
			ExecuteNextCommand();
			break;
		}
	default:
		{
			break;
		}
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
