// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGameState.h"

#include "Net/UnrealNetwork.h"

void ARoomGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	if(OnPlayerStateAdded.IsBound())
	{
		OnPlayerStateAdded.Broadcast(PlayerState);
	}
}

void ARoomGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	if(OnPlayerStateRemoved.IsBound())
	{
		OnPlayerStateRemoved.Broadcast(PlayerState);
	}
}

void ARoomGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARoomGameState, CurrentMap);
}

void ARoomGameState::SetCurrentMap(const FMapInfo& NewMap)
{
	if (HasAuthority())
	{
		CurrentMap = NewMap; 
	}
}
