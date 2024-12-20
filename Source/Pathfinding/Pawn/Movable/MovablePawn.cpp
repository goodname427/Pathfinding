// Fill out your copyright notice in the Description page of Project Settings.


#include "MovablePawn.h"

#include "PFUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Movement/ConsciousPawnMovementComponent.h"


// Sets default values
AMovablePawn::AMovablePawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = INIT_DEFAULT_SUBOBJECT(CapsuleComponent);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->InitCapsuleSize(34.0f, 88.0f);
	CapsuleComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	CapsuleComponent->CanCharacterStepUpOn = ECB_No;
	CapsuleComponent->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponent->SetCanEverAffectNavigation(false);
	CapsuleComponent->bDynamicObstacle = true;

	LocationToGroundOffset = CapsuleComponent->GetScaledCapsuleHalfHeight();

	MovementComponent = CreateDefaultSubobject<UPawnMovementComponent, UConsciousPawnMovementComponent>(
		TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;
	
	StaticMeshComponent->AlwaysLoadOnClient = true;
	StaticMeshComponent->AlwaysLoadOnServer = true;
	StaticMeshComponent->bCastDynamicShadow = true;
	StaticMeshComponent->bAffectDynamicIndirectLighting = true;
	StaticMeshComponent->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	StaticMeshComponent->SetupAttachment(CapsuleComponent);
	static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
	StaticMeshComponent->SetCollisionProfileName(MeshCollisionProfileName);
	StaticMeshComponent->SetGenerateOverlapEvents(false);
	StaticMeshComponent->SetCanEverAffectNavigation(false);

	INIT_DEFAULT_SUBOBJECT(MoveCommandComponent);
}

UCommandComponent* AMovablePawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return MoveCommandComponent;
}
