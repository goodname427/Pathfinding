// Fill out your copyright notice in the Description page of Project Settings.


#include "GatherCommandComponent.h"

#include "ConsciousPawn.h"
#include "SpawnCommandComponent.h"

FName UGatherCommandComponent::StaticCommandName = FName("Gather");

UGatherCommandComponent::UGatherCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.CommandName = StaticCommandName;
	Data.RequiredTargetRadius = -1;

	GatherFlagMeshComponent = nullptr;
}

void UGatherCommandComponent::InternalBeginExecute_Implementation()
{
	const TArray<UCommandComponent*>& SpawnCommands = GetExecutePawn()->GetCommandsByName(USpawnCommandComponent::StaticCommandName);
	for (UCommandComponent* Command : SpawnCommands)
	{
		if (USpawnCommandComponent* SpawnCommand = Cast<USpawnCommandComponent>(Command))
		{
			if (GatherFlagMeshComponent != nullptr)
			{
				GatherFlagMeshComponent->SetWorldLocation(Request.TargetLocation);
			}
			
			SpawnCommand->SetGatherLocation(Request.TargetLocation);
		}
	}

	EndExecute(ECommandExecuteResult::Success);
}

