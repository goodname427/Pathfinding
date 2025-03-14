// Fill out your copyright notice in the Description page of Project Settings.


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
	Data.bCommandEnableCheckBeforeExecute = false;
}

void USpawnCommandComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (const AConsciousPawn* CDO = GetDefaultObjectToSpawn())
	{
		if (!CDO->GetConsciousData().IsAllowedToSpawn())
		{
			PawnClassToSpawn = nullptr;
		}
	}
}

float USpawnCommandComponent::GetProgressDuration_Implementation() const
{
	return PawnClassToSpawn.Get()
		       ? PawnClassToSpawn.GetDefaultObject()->GetConsciousData().CreateDuration
		       : -1;
}

const AConsciousPawn* USpawnCommandComponent::GetDefaultObjectToSpawn() const
{
	return PawnClassToSpawn.Get() ? PawnClassToSpawn.GetDefaultObject() : nullptr;
}

FName USpawnCommandComponent::GetCommandName_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToSpawn();
	return FName(FString::Printf(TEXT("Spawn %s"), CDO ? *CDO->GetData().Name.ToString() : TEXT("")));
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

bool USpawnCommandComponent::InternalIsCommandEnable_Implementation() const
{
	if (!Super::InternalIsCommandEnable_Implementation())
	{
		return false;
	}

	if (!PawnClassToSpawn.Get())
	{
		return false;
	}

	const ABattlePlayerState* PS = GetExecutePlayerState();
	if (PS == nullptr)
	{
		return false;
	}

	const FConsciousData& ConsciousData = PawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	
	return PS->IsResourceEnough(ConsciousData.ResourceCost);
}

void USpawnCommandComponent::InternalPushedToQueue_Implementation()
{
	AUTHORITY_CHECK();
	
	const FConsciousData& ConsciousData = PawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePlayerState();

	// Consume resources
	PS->TakeResource(this, EResourceTookReason::Spawn, ConsciousData.ResourceCost);
}

void USpawnCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	AUTHORITY_CHECK();
	
	const FConsciousData& ConsciousData = PawnClassToSpawn.GetDefaultObject()->GetConsciousData();
	ABattlePlayerState* PS = GetExecutePlayerState();

	// Return resources
	PS->TakeResource(this, EResourceTookReason::Return, ConsciousData.ResourceCost);
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
			// DEBUG_MESSAGE(TEXT("Spawn [%s] at [%s]"), *PawnClassToSpawn->GetClass()->GetName(),
			//               *ExecutePawn->GetActorLocation().ToString());
			AConsciousPawn* Pawn = Commander->SpawnPawnFrom<AConsciousPawn>(ExecutePawn, PawnClassToSpawn);
			if (Pawn)
			{
				Pawn->Receive(FTargetRequest::Make<UMoveCommandComponent>(GatherLocation));
			}
			else
			{
				const FConsciousData& ConsciousData = PawnClassToSpawn.GetDefaultObject()->GetConsciousData();
				ABattlePlayerState* PS = GetExecutePlayerState();

				// Return resources
				PS->TakeResource(this, EResourceTookReason::Return, ConsciousData.ResourceCost);
			}
		}
	}
}
