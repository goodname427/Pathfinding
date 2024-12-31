// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousAIController.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "CookOnTheSide/CookOnTheFlyServer.h"

AConsciousAIController::AConsciousAIController()
{
	PrimaryActorTick.bCanEverTick = false;

	// Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}


