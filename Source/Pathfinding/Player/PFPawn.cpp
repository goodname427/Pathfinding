// Fill out your copyright notice in the Description page of Project Settings.

#include "PFPawn.h"

#include "CommanderPawn.h"
#include "PFBlueprintFunctionLibrary.h"
#include "PFUtils.h"
#include "Net/UnrealNetwork.h"


// Sets default values
APFPawn::APFPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	RootComponent = INIT_DEFAULT_SUBOBJECT(StaticMesh);
}

void APFPawn::BeginPlay()
{
	Super::BeginPlay();
	UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(StaticMesh, MaterialParent, 0);
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetOwnerColor());
}

void APFPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerPlayer);
}

void APFPawn::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewOwner);
	if (CommanderPawn != nullptr)
	{
		OwnerPlayer = CommanderPawn->GetPlayerState<APFPlayerState>();
		OnRep_OwnerPlayer();
	}
}

EPawnRole APFPawn::GetPawnRole(APFPawn* OtherPawn) const
{
	if (OtherPawn == nullptr)
	{
		return EPawnRole::None;
	}

	const APFPlayerState* OtherOwnerPlayer = OtherPawn->GetOwnerPlayer();
	if (OwnerPlayer == nullptr || OtherOwnerPlayer == nullptr)
	{
		return EPawnRole::Neutrality;
	}
	
	if (OwnerPlayer == OtherOwnerPlayer)
	{
		return EPawnRole::Self;
	}

	if (OwnerPlayer->GetTeamId() == OtherOwnerPlayer->GetTeamId())
	{
		return EPawnRole::Teammate;
	}

	return EPawnRole::None;
}

void APFPawn::OnRep_OwnerPlayer()
{
	// DEBUG_MESSAGE(TEXT("APFPawn::OnRep_OwnerPlayer [%s]"), *OwnerPlayer->GetPlayerName());
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetOwnerColor());
}

void APFPawn::OnSelected(ACommanderPawn* SelectCommander)
{
	bSelected = true;
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetDefault<UPFGameSettings>()->PawnSelectedColor);
}

void APFPawn::OnDeselected()
{
	bSelected = false;
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMesh, GetOwnerColor());
}



