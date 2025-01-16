// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingFramePawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ABuildingFramePawn::ABuildingFramePawn()
{
	bReplicates = false;

	StaticMeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	BoxComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	
	INIT_DEFAULT_SUBOBJECT(BuildingCommandComponent);

	MaxHealth = 1;
	Attack = 0;
	AttackSpeed = 0;
	Defense = 1;
}

void ABuildingFramePawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BuildingClassToBuild);
	DOREPLIFETIME(ThisClass, Commander);
}

bool ABuildingFramePawn::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	return bReplicates && Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}

void ABuildingFramePawn::SetColor(const FLinearColor& InColor)
{
	UPFBlueprintFunctionLibrary::TryCreateDynamicMaterialInstanceForStaticMesh(
		StaticMeshComponent, GetDefault<UPFGameSettings>()->LoadBuildingTranslucentMaterial());
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, InColor);
}

void ABuildingFramePawn::OnTarget(class ACommanderPawn* TargetCommander)
{
	// no response
	Super::OnTarget(TargetCommander);
}

void ABuildingFramePawn::SetBuildingClassToBuild(TSubclassOf<ABuildingPawn> InBuildingClassToBuild, ACommanderPawn* InCommander)
{
	NULL_CHECK(InBuildingClassToBuild.Get());
	NULL_CHECK(InCommander);

	BuildingClassToBuild = InBuildingClassToBuild;
	Commander = InCommander;

	OnRep_BuildingClassToBuild();
}

void ABuildingFramePawn::OnRep_BuildingClassToBuild()
{
	NULL_CHECK(BuildingClassToBuild);
	
	const ABuildingPawn* CDO = BuildingClassToBuild->GetDefaultObject<ABuildingPawn>();
	const UStaticMeshComponent* CDOMeshComponent = CDO->GetStaticMeshComponent();

	StaticMeshComponent->SetRelativeLocation(CDOMeshComponent->GetRelativeLocation());
	StaticMeshComponent->SetRelativeRotation(CDOMeshComponent->GetRelativeRotation());
	StaticMeshComponent->SetRelativeScale3D(CDOMeshComponent->GetRelativeScale3D());

	StaticMeshComponent->SetStaticMesh(CDOMeshComponent->GetStaticMesh());

	const FPFPawnData& DataOfBuildingToBuild = CDO->GetData();
	Data.Name = DataOfBuildingToBuild.Name;
	Data.Description = DataOfBuildingToBuild.Description;
	Data.Icon = DataOfBuildingToBuild.Icon;
}
