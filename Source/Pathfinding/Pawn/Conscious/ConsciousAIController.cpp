// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "PFGameSettings.h"
#include "BehaviorTree/BlackboardComponent.h"

FName AConsciousAIController::CurrentCommandKeyName = FName("CurrentCommand");

AConsciousAIController::AConsciousAIController()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AConsciousAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AConsciousAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	RunBehaviorTree(Cast<UBehaviorTree>(GetDefault<UPFGameSettings>()->ConsciousPawnBehaviorTree.ResolveObject()));
}

UCommandComponent* AConsciousAIController::GetCommand() const
{
	return Cast<UCommandComponent>(Blackboard->GetValueAsObject(CurrentCommandKeyName));
}

void AConsciousAIController::ExecuteCommand(UCommandComponent* Command)
{
	Blackboard->SetValueAsObject(CurrentCommandKeyName, Command);
}

void AConsciousAIController::CancelCommand()
{
	Blackboard->SetValueAsObject(CurrentCommandKeyName, nullptr);
}
