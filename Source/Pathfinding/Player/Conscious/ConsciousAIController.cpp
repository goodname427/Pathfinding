// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "PFGameSettings.h"

AConsciousAIController::AConsciousAIController()
{
	PrimaryActorTick.bCanEverTick = true;

	BehaviorTree = nullptr;
}

void AConsciousAIController::BeginPlay()
{
	Super::BeginPlay();

	BehaviorTree = Cast<UBehaviorTree>(GetDefault<UPFGameSettings>()->ConsciousBehaviorTree.ResolveObject());
}

void AConsciousAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	RunBehaviorTree(BehaviorTree);
}

