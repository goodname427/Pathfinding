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
