// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovablePawn.h"
#include "Component/CollectorComponent.h"
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
	void CollectOrTransportResource(AResourcePawn* CurrentCollectedResource);
	
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

protected:
	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectCommandComponent* CollectCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UTransportCommandComponent* TransportCommandComponent;

	UPROPERTY(Category = "Collect", VisibleAnywhere)
	UCollectorComponent* CollectorComponent;

	UPROPERTY(Transient)
	AResourcePawn* NextResourceToCollect;
};
