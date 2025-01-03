// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPlayerState.h"
#include "BattlePlayerState.generated.h"

class ABaseCampPawn;
class APFPawn;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0,
	Coin = 1,
};

constexpr int32 GNumResourceType = 1;

USTRUCT(BlueprintType)
struct FResourceInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EResourceType Type;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Point;

	FResourceInfo()
	{
		Type = EResourceType::None;
		Point = 0;
	}

	FResourceInfo(TTuple<EResourceType, int32> InTuple)
	{
		Type = InTuple.Get<0>();
		Point = InTuple.Get<1>();
	}

	bool IsValid() const { return Type != EResourceType::None && Point > 0; }

	void Empty()
	{
		Type = EResourceType::None;
		Point = 0;
	}
};

UENUM()
enum class EResourceTookReason : uint8
{
	Spawn = 0,
	Collect = 128,
	Return = 129,
};

/**
 * 
 */
UCLASS()
class PATHFINDING_API ABattlePlayerState : public APFPlayerState
{
	GENERATED_BODY()

	friend class ABaseCampPawn;

public:
	ABattlePlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintCallable)
	int32 GetResource(EResourceType ResourceType) const { return Resources[static_cast<int32>(ResourceType) - 1]; }

	UFUNCTION(BlueprintCallable)
	bool IsResourceEnough(const FResourceInfo& ResourceInfo) const;
	
	// Server only
	UFUNCTION(BlueprintCallable)
	void TakeResource(UObject* Source, EResourceTookReason TookReason, const FResourceInfo& ResourceInfo);

protected:
	void SetResource(EResourceType ResourceType, int32 InValue)
	{
		Resources[static_cast<int32>(ResourceType) - 1] = InValue;
	}

protected:
	UPROPERTY(Transient, Replicated)
	TArray<int32> Resources;

public:
	void OnPlayerOwnedPawnAdd(APFPawn* Pawn);
	void OnPlayerOwnedPawnRemoved(APFPawn* Pawn);

	UFUNCTION(BlueprintCallable)
	ABaseCampPawn* GetFirstBaseCamp() const;

	UFUNCTION(BlueprintCallable)
	ABaseCampPawn* GetNearestBaseCamp(AActor* Actor) const;

protected:
	UPROPERTY(Transient, Replicated)
	TArray<ABaseCampPawn*> BaseCamps;
};
