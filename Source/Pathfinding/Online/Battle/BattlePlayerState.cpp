// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerState.h"

#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Net/UnrealNetwork.h"

ABattlePlayerState::ABattlePlayerState()
{
	Resources.SetNum(GNumResourceType);

	if (Resources.Num() > 0)
	{
		Resources[0] = 100;
	}
}

void ABattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Resources);
	DOREPLIFETIME(ThisClass, BaseCamps);
}

bool ABattlePlayerState::IsResourceEnough(const FResourceInfo& ResourceInfo) const
{
	if (ResourceInfo.Type == EResourceType::None)
	{
		return true;
	}

	return GetResource(ResourceInfo.Type) >= ResourceInfo.Point;
}

void ABattlePlayerState::TakeResource(UObject* Source, EResourceTookReason TookReason,
                                      const FResourceInfo& ResourceInfo)
{
	if (!HasAuthority())
	{
		return;
	}

	if (ResourceInfo.Type == EResourceType::None)
	{
		return;
	}
	
	// add resource
	if (TookReason >= EResourceTookReason::Collect)
	{
		SetResource(ResourceInfo.Type, GetResource(ResourceInfo.Type) + ResourceInfo.Point);
		return;
	}

	// remove resource
	SetResource(ResourceInfo.Type, GetResource(ResourceInfo.Type) - ResourceInfo.Point);
}

void ABattlePlayerState::OnPlayerOwnedPawnAdd(APFPawn* Pawn)
{
	if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(Pawn))
	{
		BaseCamps.Add(BaseCampPawn);
	}
}

void ABattlePlayerState::OnPlayerOwnedPawnRemoved(APFPawn* Pawn)
{
	if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(Pawn))
	{
		BaseCamps.Remove(BaseCampPawn);
	}
}

ABaseCampPawn* ABattlePlayerState::GetFirstBaseCamp() const
{
	if (BaseCamps.Num() > 0)
	{
		return BaseCamps[0];
	}

	return nullptr;
}

ABaseCampPawn* ABattlePlayerState::GetNearestBaseCamp(AActor* Actor) const
{
	if (BaseCamps.Num() == 0)
	{
		return nullptr;
	}

	const FVector Location = Actor->GetActorLocation();
	ABaseCampPawn* NearestBaseCamp = BaseCamps[0];
	float Distance = FVector::Dist(Location, NearestBaseCamp->GetActorLocation());

	for (int32 i = 1; i < BaseCamps.Num(); i++)
	{
		const float CurrentDistance = FVector::Dist(Location, BaseCamps[i]->GetActorLocation());
		if (CurrentDistance < Distance)
		{
			NearestBaseCamp = BaseCamps[i];
			Distance = CurrentDistance;
		}
	}

	return NearestBaseCamp;
}
