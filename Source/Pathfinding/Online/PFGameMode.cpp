// Copyright Epic Games, Inc. All Rights Reserved.


#include "PFGameMode.h"

#include "GameMapsSettings.h"
#include "PFGameSession.h"
#include "PFGameState.h"
#include "PFHUD.h"
#include "PFPlayerController.h"
#include "PFPlayerState.h"

APFGameMode::APFGameMode()
{
	GameSessionClass = APFGameSession::StaticClass();
	GameStateClass = APFGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<APFPlayerController> PlayerControllerClassFinder(TEXT("/Game/Blueprints/Player/BP_PFPlayerController"));
	if (PlayerControllerClassFinder.Succeeded())
	{
		PlayerControllerClass = PlayerControllerClassFinder.Class;
	}

	PlayerStateClass = APFPlayerState::StaticClass();
	HUDClass = APFHUD::StaticClass();
}

void APFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}
