﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameInstance.h"
#include "PFGameMode.h"
#include "RoomGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ARoomGameMode : public APFGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};