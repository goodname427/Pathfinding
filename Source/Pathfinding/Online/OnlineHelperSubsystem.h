// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OnlineHelperSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UOnlineHelperSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

public:
	// Network
	UFUNCTION(BlueprintCallable)
	void DestroySession();
	
	UFUNCTION(BlueprintCallable)
	void HostRoom();

	UFUNCTION(BlueprintCallable)
	void JoinRoom();

	UFUNCTION(BlueprintCallable)
	void FindRooms();
};
