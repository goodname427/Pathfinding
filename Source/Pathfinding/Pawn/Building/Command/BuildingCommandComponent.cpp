// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingCommandComponent.h"

#include "ConsciousPawn.h"
#include "PFUtils.h"
#include "Building/BuildingPawn.h"

FName UBuildingCommandComponent::StaticCommandName = FName("Building");

UBuildingCommandComponent::UBuildingCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.bNeedToTarget = false;
}

bool UBuildingCommandComponent::InternalIsCommandEnable_Implementation() const
{
	return GetExecutePawn<ABuildingPawn>() != nullptr && GetExecutePlayerState() != nullptr;
}

float UBuildingCommandComponent::GetProgressDuration_Implementation() const
{
	return GetExecutePawn()->GetConsciousData().CreateDuration;
}

UObject* UBuildingCommandComponent::GetCommandIcon_Implementation() const
{
	return GetExecutePawn()->GetData().Icon;
}

void UBuildingCommandComponent::InternalPushedToQueue_Implementation()
{
	DEBUG_FUNC_FLAG();
	
	AUTHORITY_CHECK();

	const AConsciousPawn* ExecutePawn = GetExecutePawn();
	const FConsciousData& ConsciousData = ExecutePawn->GetConsciousData();
	ABattlePlayerState* PS = ExecutePawn->GetOwnerPlayer();

	// Consume resources
	PS->TakeResource(this, EResourceTookReason::Build, ConsciousData.ResourceCost);
}

void UBuildingCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	AUTHORITY_CHECK();

	AConsciousPawn* ExecutePawn = GetExecutePawn();
	const FConsciousData& ConsciousData = ExecutePawn->GetConsciousData();
	ABattlePlayerState* PS = ExecutePawn->GetOwnerPlayer();

	// Return resources
	PS->TakeResource(this, EResourceTookReason::Return, ConsciousData.ResourceCost);

	ExecutePawn->Destroy();
}

void UBuildingCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AUTHORITY_CHECK();

	if (Result == ECommandExecuteResult::Success)
	{
		ABuildingPawn* BuildingPawn = GetExecutePawn<ABuildingPawn>();
		if (BuildingPawn)
		{
			BuildingPawn->EndBuilding();
		}
	}
}
