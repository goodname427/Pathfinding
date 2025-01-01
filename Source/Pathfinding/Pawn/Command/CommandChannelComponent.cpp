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

		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandToPop);
		OnRep_CommandQueue();
	}
}

void UCommandChannelComponent::ClearCommands()
{
	AbortCurrentCommand();

	for (const FCommandWrapper& CommandWrapper : CommandQueue)
	{
		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CommandWrapper.Command);
	}

	CommandQueue.Empty();
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
		ClearCommands();
	}
}

void UCommandChannelComponent::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		GetConsciousPawnOwner()->DispatchCommand_OnPoppedFromQueue(CurrentCommand);
		GetConsciousPawnOwner()->DispatchCommand_EndExecute(CurrentCommand, ECommandExecuteResult::Aborted);

		CurrentCommand = nullptr;
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
		ClearCommands();
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
