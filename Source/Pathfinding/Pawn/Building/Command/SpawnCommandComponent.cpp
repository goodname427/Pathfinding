﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnCommandComponent.h"

#include "CommanderPawn.h"
#include "ConsciousPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"

FName USpawnCommandComponent::StaticCommandName = FName("Spawn");

USpawnCommandComponent::USpawnCommandComponent()
{
	Data.Name = StaticCommandName;
	Data.bNeedToTarget = false;
	Data.bArgumentsValidCheckBeforeExecute = false;
}

void USpawnCommandComponent::PostInitProperties()
{
	Super::PostInitProperties();

	// if (const AConsciousPawn* CDO = GetDefaultObjectToSpawn())
	// {
	// 	if (!CDO->GetConsciousData().IsAllowedToSpawn())
	// 	{
	// 		ConsciousPawnClassToSpawn = nullptr;
	// 	}
	// }
}

void USpawnCommandComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const AConsciousPawn* CDO = GetDefaultObjectToSpawn())
	{
		if (!CDO->GetConsciousData().IsAllowedToSpawn())
		{
			ConsciousPawnClassToSpawn = nullptr;
		}
	}
}

float USpawnCommandComponent::GetProgressDuration_Implementation() const
{
	return ConsciousPawnClassToSpawn.Get()
		       ? ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData().SpawnDuration
		       : -1;
}

const AConsciousPawn* USpawnCommandComponent::GetDefaultObjectToSpawn() const
{
	return ConsciousPawnClassToSpawn.Get() ? ConsciousPawnClassToSpawn.GetDefaultObject() : nullptr;
}

FString USpawnCommandComponent::GetCommandDisplayName_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToSpawn();
	return FString::Printf(TEXT("Spawn %s"), CDO ? *CDO->GetData().Name.ToString() : TEXT(""));
}

FString USpawnCommandComponent::GetCommandDescription_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToSpawn();
	return CDO? CDO->GetData().Description : TEXT("");
}

UObject* USpawnCommandComponent::GetCommandIcon_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToSpawn();
	return CDO? CDO->GetData().Icon : nullptr;
}

bool USpawnCommandComponent::InternalIsArgumentsValid_Implementation()
{
	if (!Super::InternalIsArgumentsValid_Implementation())
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

	const FConsciousData& ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	
	return ConsciousData.IsResourcesEnough(PS);
}

void USpawnCommandComponent::InternalPushedToQueue_Implementation()
{
	AUTHORITY_CHECK();
	
	const FConsciousData& ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();

	// Consume resources
	ConsciousData.ConsumeResources(this, PS);
}

void USpawnCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	AUTHORITY_CHECK();
	
	const FConsciousData& ConsciousData = ConsciousPawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();

	// Return resources
	ConsciousData.ReturnResources(this, PS);
}

void USpawnCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AUTHORITY_CHECK();
	
	if (Result == ECommandExecuteResult::Success)
	{
		AConsciousPawn* ExecutePawn = GetExecutePawn();
		ACommanderPawn* Commander = ExecutePawn->GetOwner<ACommanderPawn>();

		if (Commander)
		{
			// compute spawn location
			// DEBUG_MESSAGE(TEXT("Spawn [%s] at [%s]"), *ConsciousPawnClassToSpawn->GetClass()->GetName(),
			//               *ExecutePawn->GetActorLocation().ToString());
			
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
