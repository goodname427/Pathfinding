// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"

#include <string>

#include "BattleGameState.h"
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"

ABattleGameMode::ABattleGameMode()
{
	GameStateClass = ABattleGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassFinder(TEXT("/Game/Blueprints/Player/Pawn/BP_CommanderPawn"));
	if (DefaultPawnClassFinder.Succeeded())
	{
		DefaultPawnClass = DefaultPawnClassFinder.Class;
	}
}

AActor* ABattleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	APFPlayerState* PlayerState = Cast<APFPlayerState>(Player->PlayerState);
	if (!PlayerState)
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}
	
	// Choose a player start
	FName PlayerStartTag = FName(FString::FromInt(PlayerState->PlayerLocation));
	APlayerStart* FoundPlayerStart = nullptr;
	UWorld* World = GetWorld();
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		APlayerStart* PlayerStart = *It;

		if (PlayerStart->PlayerStartTag == PlayerStartTag)
		{
			FoundPlayerStart = PlayerStart;
			break;
		}
	}

	return FoundPlayerStart;
}
