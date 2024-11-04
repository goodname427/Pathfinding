// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPawn.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem/Public/NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"
#include "GameFramework/PawnMovementComponent.h"
#include "PFUtils.h"


// Sets default values
APFPawn::APFPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = INIT_DEFAULT_SUBOBJECT(StaticMesh);
}

// Called when the game starts or when spawned
void APFPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APFPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APFPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}



