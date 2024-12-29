// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnCommandComponent.h"

#include "CommanderPawn.h"
#include "ConsciousPawn.h"
#include "PFUtils.h"

FName USpawnCommandComponent::StaticCommandName = FName("Spawn");

USpawnCommandComponent::USpawnCommandComponent()
{
	Data.CommandName = StaticCommandName;
	Data.bNeedToTarget = false;
}

float USpawnCommandComponent::GetProgressDuration_Implementation() const
{
	return ConsciousPawnClassToSpawn.Get()
		       ? ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData().SpawnDuration
		       : -1;
}

bool USpawnCommandComponent::InternalIsReachable_Implementation()
{
	if (!Super::InternalIsReachable_Implementation())
	{
		return false;
	}

	if (!ConsciousPawnClassToSpawn.Get())
	{
		return false;
	}

	const ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();
	if (PS == nullptr)
	{
		return false;
	}

	FConsciousData ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	for (const auto Resource : ConsciousData.ResourcesToAmount)
	{
		if (Resource.Key == EResourceType::None)
		{
			continue;
		}

		if (PS->GetResource(Resource.Key) < Resource.Value)
		{
			return false;
		}
	}

	return true;
}

void USpawnCommandComponent::InternalBeginExecute_Implementation()
{
	Super::InternalBeginExecute_Implementation();

	FConsciousData ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();

	// Consume resources
	for (const auto Resource : ConsciousData.ResourcesToAmount)
	{
		if (Resource.Key == EResourceType::None)
		{
			continue;
		}

		PS->AddResource(Resource.Key, -Resource.Value);
	}
}

void USpawnCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Success)
	{
		ACommanderPawn* Commander = GetExecutePawn()->GetOwner<ACommanderPawn>();

		if (Commander)
		{
			Commander->SpawnPawn(ConsciousPawnClassToSpawn,
			                     GetExecutePawn()->GetActorLocation());
		}
	}
	else if (Result == ECommandExecuteResult::Aborted)
	{
		FConsciousData ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
		ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();

		// Return resources
		for (const auto Resource : ConsciousData.ResourcesToAmount)
		{
			if (Resource.Key == EResourceType::None)
			{
				continue;
			}

			PS->AddResource(Resource.Key, Resource.Value);
		}
	}
}
