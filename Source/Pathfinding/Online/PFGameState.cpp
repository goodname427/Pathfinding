// Fill out your copyright notice in the Description page of Project Settings.


#include "PFGameState.h"

#include "Net/UnrealNetwork.h"

void APFGameState::InitPlayerLocations(int32 MaxPlayer)
{
	if (!HasAuthority())
	{
		return;
	}
	
	PlayerLocations.SetNum(MaxPlayer);

	APFPlayerState* FirstPlayer = Cast<APFPlayerState>(PlayerArray[0]);
	if (FirstPlayer != nullptr)
	{
		SetPlayerLocation(0, FirstPlayer);
	}
}

void APFGameState::SetPlayerLocation(int32 InLocation, APFPlayerState* Player)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (InLocation < 0 || InLocation >= PlayerLocations.Num())
	{
		return;
	}

	if (PlayerLocations[InLocation] == Player && (Player == nullptr || Player->PlayerLocation == InLocation))
	{
		return;
	}
	
	PlayerLocations[InLocation] = Player;
	if (Player)
	{
		Player->PlayerLocation = InLocation;
	}

	OnRep_PlayerLocations();
}

void APFGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APFGameState, PlayerLocations);
}

void APFGameState::OnRep_PlayerLocations()
{
	if (OnPlayerLocationChanged.IsBound())
	{
		OnPlayerLocationChanged.Broadcast();
	}
}