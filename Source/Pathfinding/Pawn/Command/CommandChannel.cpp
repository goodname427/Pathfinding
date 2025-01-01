// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandChannel.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Net/UnrealNetwork.h"

ACommandChannel::ACommandChannel(): ChannelId(GCommandChannel_Default), CurrentCommand(nullptr)
{
	bReplicates = true;
	bNetLoadOnClient = true;
}

ACommandChannel* ACommandChannel::NewCommandChannel(AConsciousPawn* Owner, int32 InChannelId)
{
	ACommandChannel* CommandChannel = Owner->GetWorld()->SpawnActor<ACommandChannel>();

	CommandChannel->SetOwner(Owner);
	CommandChannel->ChannelId = InChannelId;

	return CommandChannel;
}

void ACommandChannel::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CommandQueue);
	DOREPLIFETIME(ThisClass, CurrentCommand);
	DOREPLIFETIME(ThisClass, ChannelId);
}

void ACommandChannel::BeginPlay()
{
	Super::BeginPlay();


	if (AConsciousPawn* OwnerConsciousPawn = Cast<AConsciousPawn>(GetOwner()))
	{
		OwnerConsciousPawn->AddCommandChannel(this);
	}
}

UCommandComponent* ACommandChannel::GetCurrentCommand() const
{
	return CurrentCommand;
}

const TArray<UCommandComponent*>& ACommandChannel::GetCommandsInQueue() const
{
	static TArray<UCommandComponent*> Commands;
	Commands.Reset();

	for (const FCommandWrapper& CommandWrapper : CommandQueue)
	{
		Commands.Add(CommandWrapper.Command);
	}

	return Commands;
}

void ACommandChannel::PushCommand(UCommandComponent* CommandToPush)
{
	if (CommandToPush)
	{
		CommandQueue.Push(CommandToPush);

		PushCommandToQueue(CommandToPush);
		OnRep_CommandQueue();
	}
}

void ACommandChannel::PopCommand(int32 CommandIndexToPop)
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

		PopCommandFromQueue(CommandToPop);
		OnRep_CommandQueue();
	}
}

void ACommandChannel::ClearCommands()
{
	AbortCurrentCommand();

	for (const FCommandWrapper& CommandWrapper : CommandQueue)
	{
		PopCommandFromQueue(CommandWrapper.Command);
	}

	CommandQueue.Empty();
	OnRep_CommandQueue();
}

void ACommandChannel::ExecuteNextCommand()
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
		BeginExecuteCommand(NextCommand, NextCommand->GetRequest());
	}
	else
	{
		ClearCommands();
	}
}

void ACommandChannel::BeginExecuteCommand_Implementation(UCommandComponent* Command,
                                                         const FTargetRequest& Request)
{
	if (!Command)
		return;

	if (GetLocalRole() == ROLE_Authority)
	{
		Command->OnCommandEnd.AddDynamic(this, &ThisClass::OnCommandEnd);

		CurrentCommand = Command;
		OnRep_CurrentCommand();
	}
	else
	{
		Command->SetCommandArgs(Request);
	}


	Command->BeginExecute();
}

void ACommandChannel::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		PopCommandFromQueue(CurrentCommand);
		EndExecuteCommand(CurrentCommand, ECommandExecuteResult::Aborted);

		CurrentCommand = nullptr;
		OnRep_CurrentCommand();
	}
}

void ACommandChannel::EndExecuteCommand_Implementation(UCommandComponent* Command, ECommandExecuteResult Result)
{
	if (!Command)
		return;

	Command->EndExecute(Result);
}

void ACommandChannel::PushCommandToQueue_Implementation(UCommandComponent* Command)
{
	if (!Command)
		return;

	Command->OnPushedToQueue();
}

void ACommandChannel::PopCommandFromQueue_Implementation(UCommandComponent* Command)
{
	if (!Command)
		return;

	Command->OnPoppedFromQueue();
}

void ACommandChannel::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	EndExecuteCommand(Command, Result);

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
