// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPawn.h"
#include "Movable/CollectorPawn.h"
#include "ResourcePawn.generated.h"

USTRUCT(BlueprintType)
struct FResourceData 
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EResourceType Type;

	FResourceData()
	{
		Type = EResourceType::Coin;
	}
};

UCLASS()
class PATHFINDING_API AResourcePawn : public APFPawn
{
	GENERATED_BODY()

public:
	AResourcePawn();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	bool CollectBy(ACollectorPawn* CollectorPawn);

	EResourceType GetResourceType() const { return ResourceData.Type; };

	int32 GetResourcePoint() const { return ResourcePoint; };

protected:
	UPROPERTY(Category = "State|Resource", EditDefaultsOnly, BlueprintReadOnly)
	FResourceData ResourceData;

	UPROPERTY(Category = "State|Resource", EditAnywhere, BlueprintReadWrite, Replicated, meta = (ClampMin = 1))
	int32 ResourcePoint;
};
