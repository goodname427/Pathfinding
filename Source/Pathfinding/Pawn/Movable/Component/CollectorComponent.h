// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Battle/BattlePlayerState.h"
#include "Command/CommandComponent.h"
#include "Components/ActorComponent.h"
#include "CollectorComponent.generated.h"


class UCollectCommandComponent;
class UTransportCommandComponent;
class AConsciousPawn;
class AResourcePawn;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PATHFINDING_API UCollectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCollectorComponent();

protected:
	virtual void BeginPlay() override;

public:
	void SetCollectedResourceType(EResourceType NewResourceType);

	void SetCollectedResourcePoint(int32 NewResourcePoint)
	{
		CollectedResource.Point = FMath::Clamp(NewResourcePoint, 0, MaxCollectedResourcePoint);
	}

	void EmptyCollectedResource() { CollectedResource.Empty(); };

	const FResourceInfo& GetCollectedResource() const { return CollectedResource; }

	int32 GetMaxCollectedResourcePoint() const { return MaxCollectedResourcePoint; }

	int32 GetResourcePointPerCollecting() const { return ResourcePointPerCollecting; }

	bool IsCollectedResourceEmpty() const { return CollectedResource.Point == 0; }

	bool IsCollectedResourceFull() const { return CollectedResource.Point >= MaxCollectedResourcePoint; }

	int32 GetMaxAvailableCollectedResourcePoint() const
	{
		return FMath::Min(ResourcePointPerCollecting,
		                  FMath::Max(0, MaxCollectedResourcePoint - CollectedResource.Point));
	}

protected:
	void FindAndRecordNextResourceToCollect(AResourcePawn* CurrentCollectedResource);

	// Server only
	void CollectOrTransportResource(AResourcePawn* CurrentCollectedResource);

	// Server only
	void CollectResource();

	// Server only
	void TransportResource(bool bSkipLastTransposedBaseCamp = false);

	UFUNCTION()
	void OnReceivedRequest(AConsciousPawn* ConsciousPawn, const FTargetRequest& Request);
	
	UFUNCTION()
	void OnCollectCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);

	UFUNCTION()
	void OnCollectCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason);

	UFUNCTION()
	void OnTransportCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);

	UFUNCTION()
	void OnTransportCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason);

public:
	UPROPERTY()
	UCollectCommandComponent* CollectCommandComponent;

	UPROPERTY()
	UTransportCommandComponent* TransportCommandComponent;

	UPROPERTY()
	AConsciousPawn* Collector;

protected:
	UPROPERTY(Category = "Collector", EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	int32 ResourcePointPerCollecting;

	UPROPERTY(Category = "Collector", EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	int32 MaxCollectedResourcePoint;

	FResourceInfo CollectedResource;

	UPROPERTY(Transient)
	AResourcePawn* NextResourceToCollect;
};
