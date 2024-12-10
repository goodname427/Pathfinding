// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsciousPawn.h"

#include "PFUtils.h"


// Sets default values
AConsciousPawn::AConsciousPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AConsciousPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConsciousPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AConsciousPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AConsciousPawn::Receive(const FCommandInfo& CommandInfo) const
{
	DEBUG_MESSAGE(TEXT("Pawn [%s] Received Command [%s]"), *GetName(), *CommandInfo.CommandName.ToString());
}

