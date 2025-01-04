// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildCommandComponent.h"

#include "CommanderPawn.h"
#include "PFUtils.h"

FName UBuildCommandComponent::StaticCommandName = FName("Build");

UBuildCommandComponent::UBuildCommandComponent()
{
	Data.Name = StaticCommandName;
	// Data.RequiredTargetRadius = -1;
}

void UBuildCommandComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const AConsciousPawn* CDO = GetDefaultObjectToBuild())
	{
		if (!CDO->GetConsciousData().IsAllowedToBuild())
		{
			PawnClassToBuild = nullptr;
		}
	}
}

FString UBuildCommandComponent::GetCommandDisplayName_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToBuild();
	return FString::Printf(TEXT("Build %s"), CDO ? *CDO->GetData().Name.ToString() : TEXT(""));
}

FString UBuildCommandComponent::GetCommandDescription_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToBuild();
	return CDO ? CDO->GetData().Description : TEXT("");
}

UObject* UBuildCommandComponent::GetCommandIcon_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToBuild();
	return CDO ? CDO->GetData().Icon : nullptr;
}

bool UBuildCommandComponent::InternalIsCommandEnable_Implementation() const
{
	if (!PawnClassToBuild.Get())
	{
		return false;
	}

	const ABattlePlayerState* PS = GetExecutePawn()->GetOwnerPlayer();
	if (PS == nullptr)
	{
		return false;
	}

	const FConsciousData& ConsciousData = PawnClassToBuild.GetDefaultObject()->GetConsciousData();

	return ConsciousData.IsResourcesEnough(PS);
}

void UBuildCommandComponent::InternalBeginExecute_Implementation()
{
	AUTHORITY_CHECK();

	AConsciousPawn* ExecutePawn = GetExecutePawn();
	ACommanderPawn* Commander = ExecutePawn->GetOwner<ACommanderPawn>();

	if (Commander)
	{
		// DEBUG_MESSAGE(TEXT("Build [%s] at [%s]"), *PawnClassToBuild->GetClass()->GetName(),
		//               *Request.TargetLocation.ToString());

		GetDefaultObjectToBuild()->GetConsciousData().ConsumeResources(this, ExecutePawn->GetOwnerPlayer());
		
		Commander->SpawnPawn(
			PawnClassToBuild,
			Request.TargetLocation
		);

		EndExecute(ECommandExecuteResult::Success);
		return;
	}
	
	EndExecute(ECommandExecuteResult::Failed);
}

const AConsciousPawn* UBuildCommandComponent::GetDefaultObjectToBuild() const
{
	return PawnClassToBuild.Get() ? PawnClassToBuild.GetDefaultObject() : nullptr;
}
