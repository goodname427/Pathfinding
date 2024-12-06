// Fill out your copyright notice in the Description page of Project Settings.


#include "PFOnlineSession.h"

#include "PFGameInstance.h"
#include "GameStage/MainMenuGameStage.h"

void UPFOnlineSession::HandleDisconnect(UWorld* World, class UNetDriver* NetDriver)
{
	Super::HandleDisconnect(World, NetDriver);

	UPFGameInstance* GI = Cast<UPFGameInstance>(World->GetGameInstance());
	if (GI)
	{
		GI->TransitionToStage<FMainMenuGameStage>();
	}
}
