// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"

FName AConsciousAIController::CurrentCommandKeyName = FName("CurrentCommand");

AConsciousAIController::AConsciousAIController(): CurrentCommand(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	// Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

UCommandComponent* AConsciousAIController::GetCurrentCommand() const
{
	return CurrentCommand;
}

const TArray<UProgressCommandComponent*>& AConsciousAIController::GetProgressCommandsInQueue() const
{
	static TArray<UProgressCommandComponent*> ProgressCommands;
	ProgressCommands.Reset();

	for (UCommandComponent* Command : CommandQueue)
	{
		if (UProgressCommandComponent* ProgressCommand = Cast<UProgressCommandComponent>(Command))
		{
			ProgressCommands.Add(ProgressCommand);
		}
	}

	return ProgressCommands;
}

void AConsciousAIController::PushCommand(UCommandComponent* CommandToPush)
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

void AConsciousAIController::PopCommand(const FTargetRequest& PopRequest)
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

void AConsciousAIController::ClearCommands()
{
	AbortCurrentCommand();

	CommandQueue.Empty();
	RequestQueue.Empty();
	if (OnCommandUpdated.IsBound())
	{
		OnCommandUpdated.Broadcast(this);
	}
}

void AConsciousAIController::ExecuteNextCommand()
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

void AConsciousAIController::ExecuteCommand(UCommandComponent* Command)
{
	Command->OnCommandEnd.AddDynamic(this, &AConsciousAIController::OnCommandEnd);

	CurrentCommand = Command;
	if (OnCommandUpdated.IsBound())
	{
		OnCommandUpdated.Broadcast(this);
	}

	Command->BeginExecute();
}

void AConsciousAIController::AbortCurrentCommand()
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

void AConsciousAIController::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	Command->OnCommandEnd.RemoveDynamic(this, &AConsciousAIController::OnCommandEnd);
	CurrentCommand = nullptr;

	if (Result == ECommandExecuteResult::Success)
	{
		ExecuteNextCommand();
	}
	else
	{
		ClearCommands();
	}
}
