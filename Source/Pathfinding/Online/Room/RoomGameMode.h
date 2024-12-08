// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameInstance.h"
#include "PFGameMode.h"
#include "PFPlayerState.h"
#include "RoomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ARoomGameMode : public APFGameMode
{
	GENERATED_BODY()

public:
	ARoomGameMode();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void Logout(AController* Exiting) override;
};
