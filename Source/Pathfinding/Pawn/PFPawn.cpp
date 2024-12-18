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
	
	INIT_DEFAULT_SUBOBJECT(RootComponent);
	
	INIT_DEFAULT_SUBOBJECT(StaticMeshComponent);
	StaticMeshComponent->SetupAttachment(RootComponent);

	// StaticMeshComponent->bRenderCustomDepth = true;
}

void APFPawn::BeginPlay()
{
	Super::BeginPlay();

	if (UMaterialInterface* FlagMaterial = Cast<UMaterialInterface>(GetDefault<UPFGameSettings>()->PawnFlagMaterial.TryLoad()))
	{
		UPFBlueprintFunctionLibrary::CreateDynamicMaterialInstanceForStaticMesh(StaticMeshComponent, FlagMaterial, 0);
		UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor(), 0);
	}
}

void APFPawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, OwnerPlayer);
}

void APFPawn::SetOwner(AActor* NewOwner)
{
	if (OwnerPlayer)
	{
		OwnerPlayer->OnPlayerOwnedPawnRemoved(this);
	}
	
	Super::SetOwner(NewOwner);

	ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewOwner);
	if (CommanderPawn != nullptr)
	{
		// DEBUG_MESSAGE(TEXT("Set [%s]'s Owner To [%s]"), *GetName(), *CommanderPawn->GetName());
		OwnerPlayer = CommanderPawn->GetPlayerState<ABattlePlayerState>();

		if (OwnerPlayer)
		{
			OwnerPlayer->OnPlayerOwnedPawnAdd(this);
		}
		
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
	// DEBUG_MESSAGE(TEXT("%s__Pawn [%s] Set OwnerPlayer [%s]"), GetLocalRole() == ROLE_Authority ? TEXT("Server") : TEXT("Client"), *GetName(), OwnerPlayer ? *OwnerPlayer->GetPlayerName() : TEXT("NULL"));
	UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor(), 0);
}

void APFPawn::OnSelected(ACommanderPawn* SelectCommander)
{
	bSelected = true;
	StaticMeshComponent->SetRenderCustomDepth(true);
	// UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetDefault<UPFGameSettings>()->PawnSelectedColor);
}

void APFPawn::OnDeselected()
{
	bSelected = false;
	StaticMeshComponent->SetRenderCustomDepth(false);
	// UPFBlueprintFunctionLibrary::SetStaticMeshColor(StaticMeshComponent, GetOwnerColor());
}



