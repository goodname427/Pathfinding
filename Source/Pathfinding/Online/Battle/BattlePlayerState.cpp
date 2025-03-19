// Fill out your copyright notice in the Description page of Project Settings.


#include "BattlePlayerState.h"

#include "PFUtils.h"
#include "Algo/Transform.h"
#include "Building/BaseCampPawn.h"
#include "Building/KitchenPawn.h"
#include "Movable/Command/CostCommandComponent.h"
#include "Net/UnrealNetwork.h"

FString ToString(const FResourceInfo& ResourceInfo)
{
	return FString::Printf(
		TEXT("%s:%d"), *UEnum::GetValueAsString(ResourceInfo.Type).Replace(TEXT("EResourceType::"), TEXT("")),
		ResourceInfo.Point);
}

FString ToString(const TArray<FResourceInfo>& ResourceInfos)
{
	FString Result;
	for (int32 i = 0; i < ResourceInfos.Num(); i++)
	{
		Result += ToString(ResourceInfos[i]);
		if (i != ResourceInfos.Num() - 1)
			Result += TEXT(", ");
	}

	return Result;
}

FString ToString(const TMap<EResourceType, int32>& ResourceInfos)
{
	FString Result;
	int32 i = 0;
	for (auto Pair : ResourceInfos)
	{
		Result += ToString(FResourceInfo(Pair));
		if (i != ResourceInfos.Num() - 1)
			Result += TEXT(", ");
		i++;
	}

	return Result;
}

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
	DOREPLIFETIME(ThisClass, OwnedPawns);
	DOREPLIFETIME(ThisClass, bFailed);
	DOREPLIFETIME(ThisClass, TotalFoodProducePerSeconds);
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

	OwnedPawns.Add(PawnToAdd);

	// Update TotalFoodProducePerSeconds
	if (const AConsciousPawn* ConsciousPawn = Cast<AConsciousPawn>(PawnToAdd))
	{
		if (const UCostCommandComponent* CostCommandComponent = Cast<UCostCommandComponent>(
			ConsciousPawn->GetCommandByName(UCostCommandComponent::StaticCommandName)))
		{
			TotalFoodProducePerSeconds -= CostCommandComponent->GetCostCountPerSecond();
		}

		if (const UProduceCommandComponent* ProduceCommandComponent = Cast<UProduceCommandComponent>(
			ConsciousPawn->GetCommandByName(UProduceCommandComponent::StaticCommandName)))
		{
			TotalFoodProducePerSeconds += ProduceCommandComponent->GetProducedCountPerSecond();
		}
	}
}

void ABattlePlayerState::RemoveOwnedPawn(APFPawn* PawnToRemove)
{
	if (PawnToRemove == nullptr || bFailed)
	{
		return;
	}

	OwnedPawns.Remove(PawnToRemove);

	// Update TotalFoodProducePerSeconds
	if (const AConsciousPawn* ConsciousPawn = Cast<AConsciousPawn>(PawnToRemove))
	{
		if (const UCostCommandComponent* CostCommandComponent = Cast<UCostCommandComponent>(
			ConsciousPawn->GetCommandByName(UCostCommandComponent::StaticCommandName)))
		{
			TotalFoodProducePerSeconds += CostCommandComponent->GetCostCountPerSecond();
		}

		if (const UProduceCommandComponent* ProduceCommandComponent = Cast<UProduceCommandComponent>(
			ConsciousPawn->GetCommandByName(UProduceCommandComponent::StaticCommandName)))
		{
			TotalFoodProducePerSeconds -= ProduceCommandComponent->GetProducedCountPerSecond();
		}
	}

	// failure
	if (!HasPawn<ABaseCampPawn>())
	{
		Fail();
	}
}

bool ABattlePlayerState::HasPawn(TSubclassOf<APFPawn> PawnClass) const
{
	return GetFirstPawn(PawnClass) != nullptr;
}

APFPawn* ABattlePlayerState::GetFirstPawn(TSubclassOf<APFPawn> PawnClass) const
{
	TArray<APFPawn*> Pawns;
	for (APFPawn* Pawn : OwnedPawns)
	{
		if (Pawn->IsA(PawnClass))
		{
			return Pawn;
		}
	}

	return nullptr;
}

APFPawn* ABattlePlayerState::GetNearestPawn(AActor* Actor, TSubclassOf<APFPawn> PawnClass) const
{
	TArray<APFPawn*> Pawns;
	for (APFPawn* Pawn : OwnedPawns)
	{
		if (Pawn->IsA(PawnClass))
		{
			Pawns.Add(Pawn);
		}
	}

	if (Pawns.Num() == 0)
	{
		return nullptr;
	}

	const FVector Location = Actor->GetActorLocation();
	APFPawn* NearestBaseCamp = Pawns[0];
	float Distance = FVector::Dist(Location, NearestBaseCamp->GetActorLocation());

	for (int32 i = 1; i < Pawns.Num(); i++)
	{
		const float CurrentDistance = FVector::Dist(Location, Pawns[i]->GetActorLocation());
		if (CurrentDistance < Distance)
		{
			NearestBaseCamp = Pawns[i];
			Distance = CurrentDistance;
		}
	}

	return NearestBaseCamp;
}
