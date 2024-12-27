// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Camp.generated.h"

class ABaseCampPawn;
class AConsciousPawn;

USTRUCT(BlueprintType)
struct FDefaultPawnInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AConsciousPawn> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ClampMin=1))
	int32 Num;
};

/**
 * 
 */
UCLASS(Blueprintable)
class PATHFINDING_API UCamp : public UObject
{
	GENERATED_BODY()

public:
	static const TArray<UCamp*>& GetAllCamps();
	static const UCamp* GetRandomlyCamp();

	FString GetCampName() const { return CampName; }
	FString GetCampDescription() const { return CampDescription; }
	TArray<FDefaultPawnInfo> GetDefaultPawnInfos() const { return DefaultPawnInfos; }
	TSubclassOf<ABaseCampPawn> GetBaseCampPawnClass() const { return BaseCampPawnClass; }
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString CampName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString CampDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDefaultPawnInfo> DefaultPawnInfos;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ABaseCampPawn> BaseCampPawnClass;
};
