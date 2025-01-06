// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildCommandComponent.h"

#include "CommanderPawn.h"
#include "EditorCategoryUtils.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"

FName UBuildCommandComponent::StaticCommandName = FName("Build");

UBuildCommandComponent::UBuildCommandComponent(): FlagActor(nullptr), FlagMesh(nullptr)
{
	PrimaryComponentTick.bCanEverTick = true;

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

	return IsValidLocationToBuild(GetDefaultObjectToBuild(), Request.TargetLocation, true);
}

bool UBuildCommandComponent::IsValidLocationToBuild(const AActor* Actor, const FVector& Location,
                                                    bool bOnlyCollidingComponents) const
{
	FVector Origin;
	FVector BoxExtents;
	Actor->GetActorBounds(bOnlyCollidingComponents, Origin, BoxExtents);
	FBox ActorBounds(Origin - BoxExtents, Origin + BoxExtents);
	ActorBounds = ActorBounds.ShiftBy(Location - Actor->GetActorLocation());

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
	AUTHORITY_CHECK();

	AConsciousPawn* ExecutePawn = GetExecutePawn();
	ACommanderPawn* Commander = ExecutePawn->GetOwner<ACommanderPawn>();

	if (Commander)
	{
		// DEBUG_MESSAGE(TEXT("Build [%s] at [%s]"), *PawnClassToBuild->GetClass()->GetName(),
		//               *Request.TargetLocation.ToString());

		GetExecutePlayerState()->TakeResource(
			this,
			EResourceTookReason::Build,
			PawnClassToBuild.GetDefaultObject()->GetConsciousData().ResourceCost
		);

		ABuildingPawn* Building = Commander->SpawnPawn<ABuildingPawn>(
			PawnClassToBuild,
			Request.TargetLocation
		);
		if (Building)
		{
			Building->SetupInBuilding();
		}

		EndExecute(ECommandExecuteResult::Success);
		return;
	}

	EndExecute(ECommandExecuteResult::Failed);
}

void UBuildCommandComponent::InternalBeginTarget_Implementation()
{
	FlagActor = GetWorld()->SpawnActor<AActor>();
	if (FlagActor)
	{
		const AConsciousPawn* DefaultActor = GetDefaultObjectToBuild();

		UActorComponent* ActorComponent = FlagActor->AddComponentByClass(
			UStaticMeshComponent::StaticClass(), false, FTransform::Identity, false);
		if ((FlagMesh = Cast<UStaticMeshComponent>(ActorComponent)))
		{
			const UStaticMeshComponent* DefaultMesh = DefaultActor->GetStaticMeshComponent();

			FlagMesh->SetStaticMesh(DefaultMesh->GetStaticMesh());

			FlagMeshRelativeLocation = DefaultMesh->GetRelativeLocation();
			FlagMesh->SetWorldScale3D(DefaultMesh->GetRelativeScale3D());
			FlagMesh->SetWorldRotation(DefaultMesh->GetRelativeRotation());

			FlagMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

			if (UMaterialInterface* FlagMaterial = GetDefault<UPFGameSettings>()->LoadPawnFlagMaterial())
			{
				UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(
					FlagMesh,
					FlagMaterial,
					0
				);
			}
		}
	}
}

void UBuildCommandComponent::InternalEndTarget_Implementation()
{
	if (FlagActor)
	{
		FlagActor->Destroy();
		FlagActor = nullptr;
		FlagMesh = nullptr;
	}
}

void UBuildCommandComponent::InternalTarget_Implementation(float DeltaTime)
{
	if (FlagActor && FlagMesh)
	{
		const FRay Ray = TargetCommander->GetRayFromMousePosition();

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, Ray.Origin, Ray.PointAt(100 * 100), ECC_Visibility);

		if (Hit.bBlockingHit)
		{
			FlagMesh->SetVisibility(true);
			FlagActor->SetActorLocation(Hit.Location + FlagMeshRelativeLocation);

			const bool bValidLocation = IsValidLocationToBuild(FlagActor, FlagActor->GetActorLocation(), false);
			if (bValidLocation)
			{
				UPFBlueprintFunctionLibrary::SetStaticMeshColor(FlagMesh, {0, 1, 0, 0.2f}, 0);
			}
			else
			{
				UPFBlueprintFunctionLibrary::SetStaticMeshColor(FlagMesh, {1, 0, 0, 0.2f}, 0);
			}
		}
		else
		{
			FlagMesh->SetVisibility(false);
		}
	}
}

const ABuildingPawn* UBuildCommandComponent::GetDefaultObjectToBuild() const
{
	return PawnClassToBuild.Get() ? PawnClassToBuild.GetDefaultObject() : nullptr;
}
