// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerState.h"

#include "PFGameState.h"
#include "Controller/PFPlayerController.h"
#include "Net/UnrealNetwork.h"

void APFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APFPlayerState, PlayerLocation);
	DOREPLIFETIME(APFPlayerState, TeamId);
	DOREPLIFETIME(APFPlayerState, PlayerColor);
	DOREPLIFETIME(APFPlayerState, CampInfo);
}

void APFPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	APFPlayerState* PFPlayerState = Cast<APFPlayerState>(PlayerState);
	if (PFPlayerState)
	{
		PFPlayerState->PlayerLocation = PlayerLocation;
		PFPlayerState->TeamId = TeamId;
		PFPlayerState->PlayerColor = PlayerColor;
		PFPlayerState->CampInfo = CampInfo;
	}
}

APFPlayerController* APFPlayerState::GetPlayerController()
{
	static APFPlayerController* PlayerController = nullptr;
	
	if (!PlayerController)
	{
		for (auto PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++
		     PlayerControllerIter)
		{
			if ((*PlayerControllerIter)->PlayerState == this)
			{
				PlayerController = Cast<APFPlayerController>(*PlayerControllerIter);
				break;
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
		// can not set to the location while other player on the location
		if (GameState->GetPlayerLocation(InLocation) != nullptr)
		{
			return;
		}

		// reset before location to null while the player has set location
		if (PlayerLocation >= 0 && PlayerLocation < GameState->GetNumPlayerLocations() && GameState->
			GetPlayerLocation(PlayerLocation) == this)
		{
			GameState->SetPlayerLocation(PlayerLocation, nullptr);
		}

		GameState->SetPlayerLocation(InLocation, this);
	}
}
