// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerState.h"

#include "Net/UnrealNetwork.h"

void APFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APFPlayerState, TeamId);
	DOREPLIFETIME(APFPlayerState, PlayerColor);
}
