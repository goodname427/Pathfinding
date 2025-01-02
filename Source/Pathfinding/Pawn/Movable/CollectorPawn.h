// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePawn.h"
#include "CollectorPawn.generated.h"

class AResourcePawn;
class UTransportCommandComponent;
class UCollectCommandComponent;

UCLASS()
class PATHFINDING_API ACollectorPawn : public AMovablePawn
{
	GENERATED_BODY()

public:
	ACollectorPawn();

public:
	virtual void OnReceive_Implementation(const FTargetRequest& Request) override;

	virtual UCommandComponent* ResolveRequestWithoutName_Implementation(const FTargetRequest& Request) override;

protected:
	void FindAndRecordNextResourceToCollect(AResourcePawn* CurrentCollectedResource);
	
	// Server only
	void CollectResource();

	// Server only
	void TransportResource();

	UFUNCTION()
	void OnCollectCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);

	UFUNCTION()
	void OnCollectCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason);

	UFUNCTION()
	void OnTransportCommandEnd(UCommandComponent* CommandComponent, ECommandExecuteResult Result);

	UFUNCTION()
	void OnTransportCommandPoppedFromQueue(UCommandComponent* CommandComponent, ECommandPoppedReason Reason);

public:
	void SetCollectedResourceType(EResourceType NewResourceType);
	
	void SetCollectedResourcePoint(int32 NewResourcePoint) { CollectedResource.Point = FMath::Clamp(NewResourcePoint, 0, MaxCollectedResourcePoint); }

	void EmptyCollectedResource() { CollectedResource.Empty(); };
	
	const FResourceInfo& GetCollectedResource() const { return CollectedResource; }

	int32 GetMaxCollectedResourcePoint() const { return MaxCollectedResourcePoint; }

	int32 GetResourcePointPerCollecting() const { return ResourcePointPerCollecting; }

	bool IsCollectedResourceFull() const { return CollectedResource.Point >= MaxCollectedResourcePoint; }

	int32 GetMaxAvailableCollectedResourcePoint() const { return FMath::Min(ResourcePointPerCollecting, FMath::Max(0, MaxCollectedResourcePoint - CollectedResource.Point)); }

protected:
	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectCommandComponent* CollectCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UTransportCommandComponent* TransportCommandComponent;

	UPROPERTY(Category = "State|Collector", EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	int32 ResourcePointPerCollecting;

	UPROPERTY(Category = "State|Collector", EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	int32 MaxCollectedResourcePoint;
	
	UPROPERTY(Transient)
	AResourcePawn* NextResourceToCollect;

	FResourceInfo CollectedResource;
};
