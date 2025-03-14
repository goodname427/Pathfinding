// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherCommandComponent.h"

#include "ConsciousPawn.h"
#include "SpawnCommandComponent.h"

FName UGatherCommandComponent::StaticCommandName = FName("Gather");

UGatherCommandComponent::UGatherCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.RequiredTargetRadius = -1;
	Data.bHiddenInCommandListMenu = true;

	GatherFlagMeshComponent = nullptr;
}
 
bool UGatherCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	return Request.TargetPawn == nullptr;
}

void UGatherCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();

	FVector GatherLocation = Request.TargetLocation;

	// Ensure gather location is on ground
	FHitResult GatherHitResult;
	GetWorld()->LineTraceSingleByChannel(
		GatherHitResult,
		GatherLocation + FVector::UpVector * 100.0f,
		GatherLocation + FVector::DownVector * 100.0f,
		ECC_Visibility
	);
	if (GatherHitResult.bBlockingHit)
	{
		GatherLocation = GatherHitResult.Location;
	}
	else
	{
		EndExecute(ECommandExecuteResult::Failed);
	}
		
	if (GatherFlagMeshComponent != nullptr)
	{
		GatherFlagMeshComponent->SetWorldLocation(GatherLocation);
	}

	// Set spawn command gather location
	const TArray<UCommandComponent*>& SpawnCommands = GetExecutePawn()->GetCommandsByClass(
		USpawnCommandComponent::StaticClass());
	for (UCommandComponent* Command : SpawnCommands)
	{
		if (USpawnCommandComponent* SpawnCommand = Cast<USpawnCommandComponent>(Command))
		{
			SpawnCommand->SetGatherLocation(GatherLocation);
		}
	}

	EndExecute(ECommandExecuteResult::Success);
}
