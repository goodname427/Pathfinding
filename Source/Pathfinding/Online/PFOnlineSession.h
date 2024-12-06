// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/OnlineSession.h"
#include "PFOnlineSession.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API UPFOnlineSession : public UOnlineSession
{
	GENERATED_BODY()

public:
	virtual void HandleDisconnect(UWorld* World, class UNetDriver* NetDriver) override;
};
