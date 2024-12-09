// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APFPawn::APFPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	RootComponent = INIT_DEFAULT_SUBOBJECT(StaticMesh);
}

// Called when the game starts or when spawned
void APFPawn::BeginPlay()
{
	Super::BeginPlay();
	UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(StaticMesh, MaterialParent, 0);
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetColor());
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

void APFPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerPlayer);
}

void APFPawn::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewOwner);
	if (CommanderPawn != nullptr)
	{
		OwnerPlayer = CommanderPawn->GetPlayerState<APFPlayerState>();
		OnRep_OwnerPlayer();
	}
}

void APFPawn::OnRep_OwnerPlayer()
{
	// DEBUG_MESSAGE(TEXT("APFPawn::OnRep_OwnerPlayer [%s]"), *OwnerPlayer->GetPlayerName());
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetColor());
}

void APFPawn::OnSelected(ACommanderPawn* SelectCommander)
{
	bSelected = true;
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetDefault<UPFGameSettings>()->PawnSelectedColor);
}

void APFPawn::OnDeselected()
{
	bSelected = false;
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetColor());
}



