// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "NetworkSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UNetworkSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection);

public:
	// Network
	UFUNCTION(BlueprintCallable)
	void DestorySession();

	UFUNCTION(BlueprintCallable)
	void HostGame();
};
