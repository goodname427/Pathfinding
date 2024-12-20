// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "ConsciousPawn.h"

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

void AConsciousAIController::PushCommand(UCommandComponent* Command)
{
	if (Command)
	{
		CommandQueue.Enqueue(Command);
	}
}

void AConsciousAIController::ClearAllCommands()
{
	AbortCurrentCommand();

	CommandQueue.Empty();
}

void AConsciousAIController::ExecuteCommandQueue()
{
	ExecuteNextCommand();
}

void AConsciousAIController::ExecuteNextCommand()
{
	if (CommandQueue.IsEmpty())
	{
		return;
	}
	
	UCommandComponent* NextCommand;
	CommandQueue.Dequeue(NextCommand);

	if (NextCommand->CanExecute())
	{
		ExecuteCommand(NextCommand);
	}
	else
	{
		ClearAllCommands();	
	}
}

void AConsciousAIController::ExecuteCommand(UCommandComponent* Command)
{
	Command->OnCommandEnd.AddDynamic(this, &AConsciousAIController::OnCommandEnd);
	CurrentCommand = Command;
	Command->BeginExecute();
}

void AConsciousAIController::AbortCurrentCommand()
{
	if (CurrentCommand)
	{
		CurrentCommand->EndExecute(ECommandExecuteResult::Aborted);
		CurrentCommand = nullptr;
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
		ClearAllCommands();
	}
}
