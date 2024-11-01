// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameStage.h"
#include "PFGameInstance.h"

/**
 * 
 */
class PATHFINDING_API FMainMenuGameStage : public IGameStage
{
public:
	FMainMenuGameStage() {}

	virtual FString GetStageName() const { return TEXT("MainMenu"); }
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
};
