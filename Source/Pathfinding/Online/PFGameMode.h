// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PFGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PATHFINDING_API APFGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	APFGameMode();
	
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
