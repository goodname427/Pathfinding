// Fill out your copyright notice in the Description page of Project Settings.


#include "MovablePawn.h"

#include "PFUtils.h"
#include "Movement/ConsciousPawnMovementComponent.h"


// Sets default values
AMovablePawn::AMovablePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UConsciousPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	
	INIT_DEFAULT_SUBOBJECT(MoveCommandComponent);
}
