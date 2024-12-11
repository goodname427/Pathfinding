// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "ConsciousAIController.h"
#include "PFUtils.h"


// Sets default values
AConsciousPawn::AConsciousPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	AIControllerClass = AConsciousAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AConsciousPawn::Receive(const FCommandInfo& CommandInfo) const
{
	DEBUG_MESSAGE(TEXT("Pawn [%s] Received Command [%s]"), *GetName(), *CommandInfo.CommandName.ToString());
}

