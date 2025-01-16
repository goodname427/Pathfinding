// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingCommandComponent.h"

#include "ConsciousPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Building/BuildingFramePawn.h"
#include "Building/BuildingPawn.h"
#include "Components/BoxComponent.h"

FName UBuildingCommandComponent::StaticCommandName = FName("Building");

UBuildingCommandComponent::UBuildingCommandComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.bNeedToTarget = false;
	Data.bHiddenInCommandListMenu = true;
}

bool UBuildingCommandComponent::InternalIsCommandEnable_Implementation() const
{
	const ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	if (!Frame)
	{
		return false;
	}

	if (!Frame->GetBuildingClassToBuild().Get())
	{
		return false;
	}

	if (!Frame->GetCommander())
	{
		return false;
	}
	
	return true;
}

float UBuildingCommandComponent::GetProgressDuration_Implementation() const
{
	const ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	const AConsciousPawn* CDO = Frame ? Frame->GetDefaultObjectToBuild() : nullptr;
	return CDO ? CDO->GetConsciousData().CreateDuration : 0;
}

UObject* UBuildingCommandComponent::GetCommandIcon_Implementation() const
{
	const ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	const AConsciousPawn* CDO = Frame ? Frame->GetDefaultObjectToBuild() : nullptr;
	return CDO ? CDO->GetData().Icon : nullptr;
}

void UBuildingCommandComponent::InternalPushedToQueue_Implementation()
{
	// DEBUG_FUNC_FLAG();
	
	AUTHORITY_CHECK();

	const ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	const FConsciousData& ConsciousData = Frame->GetBuildingClassToBuild()->GetDefaultObject<AConsciousPawn>()->GetConsciousData();
	ABattlePlayerState* PS = Frame->GetCommander()->GetPlayerState<ABattlePlayerState>();

	// Consume resources
	PS->TakeResource(this, EResourceTookReason::Build, ConsciousData.ResourceCost);
}

void UBuildingCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	AUTHORITY_CHECK();

	ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	const FConsciousData& ConsciousData = Frame->GetBuildingClassToBuild()->GetDefaultObject<AConsciousPawn>()->GetConsciousData();
	ABattlePlayerState* PS = Frame->GetCommander()->GetPlayerState<ABattlePlayerState>();

	// Return resources
	PS->TakeResource(this, EResourceTookReason::Return, ConsciousData.ResourceCost);

	Frame->Die();
}

void UBuildingCommandComponent::InternalBeginExecute_Implementation()
{
	Super::InternalBeginExecute_Implementation();
	
	ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();
	Frame->GetStaticMeshComponent()->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	Frame->GetBoxComponent()->SetCollisionProfileName(APFPawn::PawnBounds_ProfileName);
	Frame->SetBoxComponentToBounds();

	if (GetOwnerRole() == ROLE_Authority)
	{
		Frame->SetOwner(Frame->GetCommander());
	}
}

void UBuildingCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	AUTHORITY_CHECK();

	if (Result == ECommandExecuteResult::Success)
	{
		ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>();

		Frame->GetCommander()->SpawnPawn(Frame->GetBuildingClassToBuild(), Frame->GetActorLocation());
	
		Frame->Die();
	}
}

void UBuildingCommandComponent::InternalExecute_Implementation(float DeltaTime)
{
	if (ABuildingFramePawn* Frame = GetExecutePawn<ABuildingFramePawn>())
	{
		FLinearColor Color = Frame->GetOwnerColor();
		Color.A = GetNormalizedProgress();
		Frame->SetColor(Color);
	}
	
	Super::InternalExecute_Implementation(DeltaTime);
}
