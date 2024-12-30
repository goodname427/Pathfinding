// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnBuildingPawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Command/GatherCommandComponent.h"
#include "Command/SpawnCommandComponent.h"


class UGatherCommandComponent;
// Sets default values
ASpawnBuildingPawn::ASpawnBuildingPawn()
{
	INIT_DEFAULT_SUBOBJECT(GatherFlagMeshComponent);
	GatherFlagMeshComponent->SetupAttachment(RootComponent);
	GatherFlagMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GatherFlagMeshComponent->SetVisibility(false);

	INIT_DEFAULT_SUBOBJECT(GatherCommandComponent);
	GatherCommandComponent->GatherFlagMeshComponent = GatherFlagMeshComponent;
}

void ASpawnBuildingPawn::BeginPlay()
{
	Super::BeginPlay();

	Receive(FTargetRequest(
		GatherCommandComponent,
		UPFBlueprintFunctionLibrary::GetRandomReachablePointOfActor(this)
	));
}

bool ASpawnBuildingPawn::HasSpawnCommand() const
{
	return Commands.Contains(USpawnCommandComponent::StaticCommandName);
}

void ASpawnBuildingPawn::OnSelected(class ACommanderPawn* SelectCommander)
{
	Super::OnSelected(SelectCommander);

	if (SelectCommander->IsOwned(this) && HasSpawnCommand())
	{
		GatherFlagMeshComponent->SetVisibility(true);
	}
}

void ASpawnBuildingPawn::OnDeselected()
{
	GatherFlagMeshComponent->SetVisibility(false);

	Super::OnDeselected();
}

UCommandComponent* ASpawnBuildingPawn::ResolveRequestWithoutName_Implementation(const FTargetRequest& Request)
{
	return GatherCommandComponent;
}
