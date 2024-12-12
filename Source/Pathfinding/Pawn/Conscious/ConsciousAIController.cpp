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

void AConsciousAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AConsciousAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	UBehaviorTree* BehaviorTree;
	if (GIsEditor)
	{
		BehaviorTree = Cast<UBehaviorTree>(GetDefault<UPFGameSettings>()->ConsciousPawnBehaviorTree.ResolveObject());
	}
	else 
	{
		BehaviorTree = Cast<UBehaviorTree>(GetDefault<UPFGameSettings>()->ConsciousPawnBehaviorTree.TryLoad());
	}
	
	if (BehaviorTree)
	{
		// if (BehaviorTree->BlackboardAsset)
		// {
		// 	Blackboard->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		// }
		// DEBUG_FUNC_FLAG();
		RunBehaviorTree(BehaviorTree);
	}
}

UCommandComponent* AConsciousAIController::GetCurrentCommand() const
{
	return Cast<UCommandComponent>(Blackboard->GetValueAsObject(CurrentCommandKeyName));
}

void AConsciousAIController::ExecuteCommand(UCommandComponent* Command)
{
	Command->OnCommandEnd.AddDynamic(this, &AConsciousAIController::OnCommandEnd);
	Blackboard->SetValueAsObject(CurrentCommandKeyName, Command);
}

void AConsciousAIController::CancelCommand()
{
	Blackboard->SetValueAsObject(CurrentCommandKeyName, nullptr);
}

void AConsciousAIController::OnCommandEnd(UCommandComponent* Command)
{
	CancelCommand();
	Command->OnCommandEnd.RemoveDynamic(this, &AConsciousAIController::OnCommandEnd);
}
