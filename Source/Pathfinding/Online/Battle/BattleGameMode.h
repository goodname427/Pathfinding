// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PFGameMode.h"
#include "BattleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API ABattleGameMode : public APFGameMode
{
	GENERATED_BODY()

public:
	ABattleGameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
