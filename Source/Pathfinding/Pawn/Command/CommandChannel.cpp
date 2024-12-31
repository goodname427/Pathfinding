// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandChannel.h"

#include "PFUtils.h"

UCommandComponent* UCommandChannel::GetCurrentCommand() const
{
	return CurrentCommand;
}

void UCommandChannel::PushCommand(UCommandComponent* CommandToPush)
{
	if (CommandToPush)
	{
		CommandQueue.Push(CommandToPush);
		RequestQueue.Push(CommandToPush->GetRequest());
		CommandToPush->OnPushedToQueue();

		if (OnCommandUpdated.IsBound())
		{
			OnCommandUpdated.Broadcast(this);
		}
	}
}

void UCommandChannel::PopCommand(const FTargetRequest& PopRequest)
{
	if (PopRequest.Guid == CurrentCommand->GetRequest().Guid)
	{
		ExecuteNextCommand();
	}
	else
	{
		// Remove the command from the queue
		int32 Index = INDEX_NONE;
		
		for (int32 i = 0; i < RequestQueue.Num(); i++)
		{
			if (RequestQueue[i].Guid == PopRequest.Guid)
			{
				Index = i;
				break;
			}
		}
		
		if (Index != INDEX_NONE)
		{
			UCommandComponent* CommandToPop = CommandQueue[Index];
			CommandQueue.RemoveAt(Index);
			RequestQueue.RemoveAt(Index);
			CommandToPop->OnPoppedFromQueue();

			if (OnCommandUpdated.IsBound())
			{
				OnCommandUpdated.Broadcast(this);
			}
		}
	}
}

void UCommandChannel::ClearCommands()
{
	AbortCurrentCommand();

	CommandQueue.Empty();
	RequestQueue.Empty();
	if (OnCommandUpdated.IsBound())
	{
		OnCommandUpdated.Broadcast(this);
	}
}

void UCommandChannel::ExecuteNextCommand()
{
	AbortCurrentCommand();

	if (CommandQueue.Num() == 0)
	{
		return;
	}

	UCommandComponent* NextCommand = CommandQueue[0];
	CommandQueue.RemoveAt(0);

	NextCommand->SetCommandArgs(RequestQueue[0]);
	RequestQueue.RemoveAt(0);

	if (NextCommand->CanExecute())
	{
		ExecuteCommand(NextCommand);
	}
	else
	{
		ClearCommands();
	}
}

void UCommandChannel::ExecuteCommand(UCommandComponent* Command)
{
	Command->OnCommandEnd.AddDynamic(this, &ThisClass::OnCommandEnd);

	CurrentCommand = Command;
	if (OnCommandUpdated.IsBound())
	{
		OnCommandUpdated.Broadcast(this);
	}

	Command->BeginExecute();
}

void UCommandChannel::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		CurrentCommand->OnPoppedFromQueue();
		CurrentCommand->EndExecute(ECommandExecuteResult::Aborted);

		CurrentCommand = nullptr;
		if (OnCommandUpdated.IsBound())
		{
			OnCommandUpdated.Broadcast(this);
		}
	}
}

void UCommandChannel::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
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