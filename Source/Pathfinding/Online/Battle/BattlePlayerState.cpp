// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerState.h"

#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "Net/UnrealNetwork.h"

ABattlePlayerState::ABattlePlayerState()
{
	Resources.SetNum(GNumResourceType);

	for (auto ResourceInfo : GetDefault<UPFGameSettings>()->InitializedResource)
	{
		TakeResource(this, EResourceTookReason::Initialize, ResourceInfo);
	}

	NetUpdateFrequency = 100;
}

void ABattlePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Resources);
	DOREPLIFETIME(ThisClass, BaseCamps);
	DOREPLIFETIME(ThisClass, OwnedPawns);
	DOREPLIFETIME(ThisClass, bFailed);
}

bool ABattlePlayerState::IsResourceEnough(const FResourceInfo& ResourceInfo) const
{
	if (ResourceInfo.Type == EResourceType::None)
	{
		return true;
	}

	return GetResource(ResourceInfo.Type) >= ResourceInfo.Point;
}

bool ABattlePlayerState::IsResourceEnough(const TArray<FResourceInfo>& ResourceInfos) const
{
	for (const auto& ResourceInfo : ResourceInfos)
	{
		if (!IsResourceEnough(ResourceInfo))
		{
			return false;
		}
	}

	return true;
}

bool ABattlePlayerState::IsResourceEnough(const TMap<EResourceType, int32>& ResourceInfos) const
{
	for (const auto& ResourceInfo : ResourceInfos)
	{
		if (!IsResourceEnough(ResourceInfo))
		{
			return false;
		}
	}

	return true;
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

void ABattlePlayerState::TakeResource(UObject* Source, EResourceTookReason TookReason,
                                      const TArray<FResourceInfo>& ResourceInfos)
{
	for (const auto& Resource : ResourceInfos)
	{
		TakeResource(Source, TookReason, Resource);
	}
}

void ABattlePlayerState::TakeResource(UObject* Source, EResourceTookReason TookReason,
                                      const TMap<EResourceType, int32>& ResourceInfos)
{
	for (const auto& Resource : ResourceInfos)
	{
		TakeResource(Source, TookReason, Resource);
	}
}

void ABattlePlayerState::Fail()
{
	if (bFailed)
	{
		return;
	}
	
	bFailed = true;
	
	for (APFPawn* OwnedPawn : OwnedPawns)
	{
		OwnedPawn->Die();
	}

	BaseCamps.Empty();
	OwnedPawns.Empty();

	if (OnPlayerFailed.IsBound())
	{
		OnPlayerFailed.Broadcast(this);
	}
}

void ABattlePlayerState::AddOwnedPawn(APFPawn* PawnToAdd)
{
	if (PawnToAdd == nullptr || bFailed)
	{
		return;
	}
	
	if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(PawnToAdd))
	{
		BaseCamps.Add(BaseCampPawn);
	}

	OwnedPawns.Add(PawnToAdd);
}

void ABattlePlayerState::RemoveOwnedPawn(APFPawn* PawnToRemove)
{
	if (PawnToRemove == nullptr || bFailed)
	{
		return;
	}
		
	if (ABaseCampPawn* BaseCampPawn = Cast<ABaseCampPawn>(PawnToRemove))
	{
		BaseCamps.Remove(BaseCampPawn);
	}

	OwnedPawns.Remove(PawnToRemove);

	// failure
	if (BaseCamps.Num() == 0)
	{
		Fail();
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
