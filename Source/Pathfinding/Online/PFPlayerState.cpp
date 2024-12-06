// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerState.h"

#include "PFGameState.h"
#include "PFPlayerController.h"
#include "Net/UnrealNetwork.h"

void APFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APFPlayerState, PlayerLocation);
	DOREPLIFETIME(APFPlayerState, TeamId);
	DOREPLIFETIME(APFPlayerState, PlayerColor);
}

APFPlayerController* APFPlayerState::GetPlayerController()
{
	if (!PlayerController)
	{
		for (auto PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++
		     PlayerControllerIter)
		{
			if ((*PlayerControllerIter)->PlayerState == this)
			{
				PlayerController = Cast<APFPlayerController>(*PlayerControllerIter);
			}
		}
	}

	return PlayerController;
}

void APFPlayerState::SetPlayerLocation_Implementation(int32 InLocation)
{
	APFGameState* GameState = Cast<APFGameState>(GetWorld()->GetGameState());
	if (GameState)
	{
		if (PlayerLocation >= 0 && PlayerLocation < GameState->GetNumPlayerLocations() && GameState->
			GetPlayerLocation(PlayerLocation) == this)
		{
			GameState->SetPlayerLocation(PlayerLocation, nullptr);
		}

		GameState->SetPlayerLocation(InLocation, this);
	}
}
