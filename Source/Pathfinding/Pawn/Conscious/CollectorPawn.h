// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePawn.h"
#include "CollectorPawn.generated.h"

class UTransportCommandComponent;
class UCollectCommandComponent;

UCLASS()
class PATHFINDING_API ACollectorPawn : public AMovablePawn
{
	GENERATED_BODY()

	friend class AResourcePawn;
	friend class ABaseCampPawn;
	
public:
	ACollectorPawn();

public:
	virtual void OnReceive_Implementation(const FTargetRequest& Request, bool bStartNewCommandQueue) override;
	
	virtual UCommandComponent* ResolveRequestWithoutName_Implementation(const FTargetRequest& Request) override;

	void FindAndRecordNextResourceToCollect(AResourcePawn* CurrentCollectedResource);

	AResourcePawn* GetNextResourceToCollect() const { return NextResourceToCollect; }
	
protected:
	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectCommandComponent* CollectCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UTransportCommandComponent* TransportCommandComponent;

	UPROPERTY(Transient)
	AResourcePawn* NextResourceToCollect;
	
	int32 CollectedResource;
	EResourceType CollectedResourceType;
};
