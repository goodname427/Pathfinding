// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ClassViewerFilter.h"
#include "PFPlayerState.h"
#include "BattlePlayerState.generated.h"

class ABaseCampPawn;
class APFPawn;

UENUM(BlueprintType)
enum class EResourceType : uint8
{
	None = 0,
	Coin = 1,
	Food = 2,
};

constexpr int32 GNumResourceType = 2;

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

	FResourceInfo(EResourceType InType, int32 InPoint)
	{
		Type = InType;
		Point = InPoint;
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

FString ToString(const FResourceInfo& ResourceInfo);
FString ToString(const TArray<FResourceInfo>& ResourceInfos);
FString ToString(const TMap<EResourceType, int32>& ResourceInfos);

UENUM()
enum class EResourceTookReason : uint8
{
	//
	// Cost
	//
	Spawn = 0,
	Build = 1,
	Fixup = 2,
	FoodCostCycle = 3,

	//
	// Gain
	//
	Collect = 128,
	Return = 129,
	Initialize = 130,
	Produce = 131
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerFailedSignature, ABattlePlayerState*, Player);

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

	bool IsResourceEnough(const TArray<FResourceInfo>& ResourceInfos) const;

	bool IsResourceEnough(const TMap<EResourceType, int32>& ResourceInfos) const;
	
	// Server only
	UFUNCTION(BlueprintCallable)
	void TakeResource(UObject* Source, EResourceTookReason TookReason, const FResourceInfo& ResourceInfo);
	
	void TakeResource(UObject* Source, EResourceTookReason TookReason, const TArray<FResourceInfo>& ResourceInfos);

	void TakeResource(UObject* Source, EResourceTookReason TookReason, const TMap<EResourceType, int32>& ResourceInfos);

	UFUNCTION(BlueprintCallable)
	float GetTotalFoodCostPerCycle() const { return TotalFoodProducePerCycle; }
	
protected:
	void SetResource(EResourceType ResourceType, int32 InValue)
	{
		Resources[static_cast<int32>(ResourceType) - 1] = InValue;
	}

protected:
	UPROPERTY(Transient, Replicated)
	TArray<int32> Resources;

	UPROPERTY(Transient, Replicated)
	float TotalFoodProducePerCycle = 0;

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerFailedSignature OnPlayerFailed;
	
	UFUNCTION(BlueprintCallable)
	bool HasFailed() const { return bFailed; }

	// [Server]
	void Fail();
	
	void AddOwnedPawn(APFPawn* PawnToAdd);
	void RemoveOwnedPawn(APFPawn* PawnToRemove);

	UFUNCTION(BlueprintCallable)
	ABaseCampPawn* GetFirstBaseCamp() const;

	UFUNCTION(BlueprintCallable)
	ABaseCampPawn* GetNearestBaseCamp(AActor* Actor) const;

protected:
	UPROPERTY(Transient, Replicated)
	bool bFailed = false;
	
	UPROPERTY(Transient, Replicated)
	TArray<ABaseCampPawn*> BaseCamps;

	UPROPERTY(Transient, Replicated)
	TArray<APFPawn*> OwnedPawns;
};
