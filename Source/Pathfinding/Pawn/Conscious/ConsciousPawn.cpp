// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "ConsciousAIController.h"
#include "PFUtils.h"


// Sets default values
AConsciousPawn::AConsciousPawn(): ConsciousAIController(nullptr)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();

	for (UActorComponent* Component : GetComponents())
	{
		UCommandComponent* Command = Cast<UCommandComponent>(Component);
		if (Command)
		{
			Commands.Add(Command->GetCommandName(), Command);
		}
	}
}

void AConsciousPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ConsciousAIController = Cast<AConsciousAIController>(NewController);
}

void AConsciousPawn::Receive(const FTargetRequest& Request)
{
	// DEBUG_MESSAGE(TEXT("Pawn [%s] Received Request [%s]"), *GetName(), *Request.CommandName.ToString());

	if (!ConsciousAIController || !ConsciousAIController->GetBlackboardComponent())
	{
		DEBUG_MESSAGE(TEXT("ConsciousAIController Is Not Ready!"));
		return;
	}
	
	if (ConsciousAIController->GetCurrentCommand() != nullptr)
	{
		ConsciousAIController->CancelCommand();
	}

	UCommandComponent* CommandToExecute = ResolveRequest(Request);

	CommandToExecute->SetCommandArgs(Request);

	if (CommandToExecute->CanExecute())
	{
		ConsciousAIController->ExecuteCommand(CommandToExecute);
	}
}

UCommandComponent* AConsciousPawn::ResolveRequest(const FTargetRequest& Request)
{
	return Commands["Move"];
}
