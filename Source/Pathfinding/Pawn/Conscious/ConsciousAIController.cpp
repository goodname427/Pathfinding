// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "PFGameSettings.h"
#include "PFUtils.h"
#include "BehaviorTree/BlackboardComponent.h"

FName AConsciousAIController::CurrentCommandKeyName = FName("CurrentCommand");

AConsciousAIController::AConsciousAIController()
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
	ExecuteCommand(NextCommand);
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
	}
}

void AConsciousAIController::OnCommandEnd(UCommandComponent* Command, ECommandExecuteResult Result)
{
	Command->OnCommandEnd.RemoveDynamic(this, &AConsciousAIController::OnCommandEnd);

	if (Result == ECommandExecuteResult::Success)
	{
		ExecuteNextCommand();
	}
	else
	{
		ClearAllCommands();
	}
}
