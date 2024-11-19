// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGameMode.h"

#include "PFGameState.h"
#include "RoomGameState.h"
#include "PFUtils.h"
#include "GameFramework/GameState.h"

void ARoomGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
}

bool ARoomGameMode::ChooseMap(const FMapInfo& InMap)
{
	ARoomGameState* RoomGameState = GetGameState<ARoomGameState>();
	NULL_CHECK_RET(RoomGameState, false);
	
	// Need Kick Some Players
	if (RoomGameState->CurrentMap.MaxPlayers < InMap.MaxPlayers)
	{
		
	}
	
	RoomGameState->SetCurrentMap(InMap);
	return true;
}
