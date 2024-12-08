// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomGameState.h"

#include "Net/UnrealNetwork.h"

void ARoomGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARoomGameState, CurrentMap);
}

void ARoomGameState::SetCurrentMap(const FMapInfo& NewMap)
{
	CurrentMap = NewMap;
	InitPlayerLocations(NewMap.MaxPlayers);
}
