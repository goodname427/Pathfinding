// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGameMode.h"

#include "RoomGameState.h"
#include "GameFramework/GameState.h"

ARoomGameMode::ARoomGameMode()
{
	// bUseSeamlessTravel = true;

	GameStateClass = ARoomGameState::StaticClass();
}

void ARoomGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	APFGameState* PFGameState = GetGameState<APFGameState>();
	if (PFGameState)
	{
		int32 NumPlayerLocations = PFGameState->GetNumPlayerLocations();

		int32 i = 0;
		for (; i < NumPlayerLocations; i++)
		{
			if (PFGameState->GetPlayerLocation(i) == nullptr)
			{
				break;
			}
		}

		if (i < NumPlayerLocations)
		{
			APFPlayerState* PFPlayerState = Cast<APFPlayerState>(NewPlayer->PlayerState);
			if (PFPlayerState)
			{
				PFGameState->SetPlayerLocation(i, PFPlayerState);
			}
		}
	}
}

void ARoomGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	APFGameState* PFGameState = GetGameState<APFGameState>();
	if (PFGameState)
	{
		APFPlayerState* PFPlayerState = Cast<APFPlayerState>(Exiting->PlayerState);
		if (PFPlayerState)
		{
			PFGameState->SetPlayerLocation(PFPlayerState->PlayerLocation, nullptr);
		}
	}
}

void ARoomGameMode::BeginPlay()
{
	Super::BeginPlay();
}
