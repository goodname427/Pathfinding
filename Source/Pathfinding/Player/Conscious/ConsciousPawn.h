// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFPawn.h"
#include "ConsciousPawn.generated.h"

USTRUCT(BlueprintType)
struct FCommandInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CommandName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APFPawn* TargetPawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TargetLocation;
};

UCLASS()
class PATHFINDING_API AConsciousPawn : public APFPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AConsciousPawn();

public:
	void Receive(const FCommandInfo& CommandInfo) const;
};
