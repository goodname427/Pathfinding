// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnCommandComponent.h"

#include "CommanderPawn.h"
#include "ConsciousPawn.h"
#include "PFBlueprintFunctionLibrary.h"
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

UObject* USpawnCommandComponent::GetProgressIcon_Implementation() const
{
	return ConsciousPawnClassToSpawn.Get()
			   ? ConsciousPawnClassToSpawn.GetDefaultObject()->GetData().Icon
			   : nullptr;
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

void USpawnCommandComponent::InternalPushedToQueue_Implementation()
{
	FConsciousData ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();

	// Consume resources
	for (const auto Resource : ConsciousData.ResourcesToAmount)
	{
		if (Resource.Key == EResourceType::None)
		{
			continue;
		}

		PS->TakeResource(this, EResourceTookReason::Spawn, Resource);
	}
}

void USpawnCommandComponent::InternalPoppedFromQueue_Implementation()
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

		PS->TakeResource(this, EResourceTookReason::Return, Resource);
	}
}

void USpawnCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (Result == ECommandExecuteResult::Success)
	{
		AConsciousPawn* ExecutePawn = GetExecutePawn();
		ACommanderPawn* Commander = ExecutePawn->GetOwner<ACommanderPawn>();

		if (Commander)
		{
			// compute spawn location
			DEBUG_MESSAGE(TEXT("Spawn [%s] at [%s]"), *ConsciousPawnClassToSpawn->GetClass()->GetName(),
			              *ExecutePawn->GetActorLocation().ToString());
			
			const FVector SpawnLocation = UPFBlueprintFunctionLibrary::GetRandomReachablePointOfActor(
				ExecutePawn,
				ConsciousPawnClassToSpawn.GetDefaultObject()->GetSimpleCollisionRadius()
			);

			Commander->SpawnPawnAndMoveToLocation(
				ConsciousPawnClassToSpawn,
				SpawnLocation,
				GatherLocation
			);
		}
	}
}
