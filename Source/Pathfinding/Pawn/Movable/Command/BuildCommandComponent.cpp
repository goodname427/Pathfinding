// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildCommandComponent.h"

#include "CommanderPawn.h"
#include "EditorCategoryUtils.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Building/BuildingFramePawn.h"

FName UBuildCommandComponent::StaticCommandName = FName("Build");

UBuildCommandComponent::UBuildCommandComponent(): FrameActor(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

	Data.Name = StaticCommandName;
	Data.bNeedEverCheckWhileMoving = false;
	Data.RequiredTargetRadius = 200;
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

		BoundsToBuild = UPFBlueprintFunctionLibrary::GetCDOActorBounds(this, PawnClassToBuild);
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

float UBuildCommandComponent::GetRequiredTargetRadius_Implementation() const
{
	const AConsciousPawn* CDO = GetDefaultObjectToBuild();
	return (CDO ? CDO->GetApproximateRadius() : 0) + Super::GetRequiredTargetRadius_Implementation();
}

bool UBuildCommandComponent::InternalIsArgumentsValid_Implementation() const
{
	if (Request.TargetPawn != nullptr)
	{
		return false;
	}

	return IsValidLocationToBuild(Request.TargetLocation);
}

bool UBuildCommandComponent::IsValidLocationToBuild(const FVector& Location) const
{
	const AActor* Actor = GetDefaultObjectToBuild();
	NULL_CHECK_RET(Actor, false);

	// FVector Origin;
	// FVector BoxExtents;
	// Actor->GetActorBounds(false, Origin, BoxExtents);
	// BoundsToBuild = {Origin - BoxExtents, Origin + BoxExtents};
	// InitLocationToBuild = FrameActor->GetActorLocation();
	//FBox ActorBounds = {Origin - BoxExtents, Origin + BoxExtents};
	const FBox ActorBounds = BoundsToBuild.ShiftBy(Location);

	return UPFBlueprintFunctionLibrary::IsLocationEmptyAndOnGround(this, ActorBounds);
}

bool UBuildCommandComponent::InternalIsCommandEnable_Implementation() const
{
	if (!PawnClassToBuild.Get())
	{
		return false;
	}

	const ABattlePlayerState* PS = GetExecutePlayerState();
	if (PS == nullptr)
	{
		return false;
	}

	const FConsciousData& ConsciousData = PawnClassToBuild.GetDefaultObject()->GetConsciousData();

	return PS->IsResourceEnough(ConsciousData.ResourceCost);
}

void UBuildCommandComponent::InternalBeginExecute_Implementation()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		TryDestroyFrameActor();
		return;
	}

	AConsciousPawn* ExecutePawn = GetExecutePawn();
	ACommanderPawn* Commander = ExecutePawn->GetOwner<ACommanderPawn>();

	if (Commander)
	{
		// Server load for client
		if (FrameActor == nullptr)
		{
			FrameActor = GetWorld()->SpawnActor<ABuildingFramePawn>();
		}

		if (FrameActor)
		{
			FrameActor->SetReplicates(true);

			FrameActor->SetActorLocation(Request.TargetLocation);
			FrameActor->SetBuildingClassToBuild(PawnClassToBuild, Commander);

			FrameActor->Receive(FTargetRequest::Make<UBuildingCommandComponent>());

			EndExecute(ECommandExecuteResult::Success);
			return;
		}
	}

	EndExecute(ECommandExecuteResult::Failed);
}

void UBuildCommandComponent::InternalEndExecute_Implementation(ECommandExecuteResult Result)
{
	if (FrameActor)
	{
		if (Result == ECommandExecuteResult::Failed)
		{
			FrameActor->Die();
		}
		FrameActor = nullptr;
	}
}

void UBuildCommandComponent::InternalPushedToQueue_Implementation()
{
	if (UPFBlueprintFunctionLibrary::IsPlayerStateLocal(GetExecutePlayerState()))
	{
		SpawnFrameActor();
		if (FrameActor)
		{
			FrameActor->SetActorLocation(Request.TargetLocation);

			FLinearColor Color = GetExecutePlayerState()->GetPlayerColor();
			Color.A = 0.5f;
			FrameActor->SetColor(Color);
		}
	}
}

void UBuildCommandComponent::InternalPoppedFromQueue_Implementation(ECommandPoppedReason Reason)
{
	TryDestroyFrameActor();
}

void UBuildCommandComponent::InternalBeginTarget_Implementation()
{
	SpawnFrameActor();
}

void UBuildCommandComponent::InternalEndTarget_Implementation(bool bCanceled)
{
	// if (bCanceled || !IsValidLocationToBuild(FrameActor->GetActorLocation()))
	// {
	TryDestroyFrameActor();
	// }
}

void UBuildCommandComponent::InternalTarget_Implementation(float DeltaTime)
{
	if (FrameActor)
	{
		const FRay Ray = TargetCommander->GetRayFromMousePosition();

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Ray.Origin, Ray.PointAt(500 * 100), ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			FrameActor->GetStaticMeshComponent()->SetVisibility(true);
			FrameActor->SetActorLocation(Hit.Location);

			const bool bValidLocation = IsValidLocationToBuild(FrameActor->GetActorLocation());
			if (bValidLocation)
			{
				FrameActor->SetColor({0, 1, 0, 0.2f});
			}
			else
			{
				FrameActor->SetColor({1, 0, 0, 0.2f});
			}
		}
		else
		{
			FrameActor->GetStaticMeshComponent()->SetVisibility(false);
		}
	}
}

void UBuildCommandComponent::SpawnFrameActor()
{
	FrameActor = GetWorld()->SpawnActor<ABuildingFramePawn>();
	if (FrameActor)
	{
		FrameActor->SetBuildingClassToBuild(PawnClassToBuild, GetExecuteCommander());
	}
}

void UBuildCommandComponent::TryDestroyFrameActor()
{
	if (FrameActor)
	{
		FrameActor->Die();
		FrameActor = nullptr;
	}
}

const ABuildingPawn* UBuildCommandComponent::GetDefaultObjectToBuild() const
{
	return PawnClassToBuild.Get() ? PawnClassToBuild.GetDefaultObject() : nullptr;
}
