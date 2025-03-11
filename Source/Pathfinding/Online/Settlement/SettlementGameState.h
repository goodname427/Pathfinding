// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameState.h"
#include "SettlementGameState.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ASettlementGameState : public APFGameState
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	bool bFailed;
};
