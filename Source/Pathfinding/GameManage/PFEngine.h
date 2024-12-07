// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameEngine.h"
#include "PFEngine.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFEngine : public UGameEngine
{
	GENERATED_BODY()

	/* Hook up specific callbacks */
	virtual void Init(IEngineLoop* InEngineLoop);
	
public:
	virtual void HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
	                                  const FString& ErrorString) override;
};
