// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"

#include <string>

#include "BattleGameState.h"
#include "BattleHUD.h"
#include "EngineUtils.h"
#include "PFUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "GameFramework/PlayerStart.h"

ABattleGameMode::ABattleGameMode()
{
	GameStateClass = ABattleGameState::StaticClass();

	HUDClass = ABattleHUD::StaticClass();
	
	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassFinder(TEXT("/Game/Blueprints/Player/Pawn/BP_CommanderPawn"));
	if (DefaultPawnClassFinder.Succeeded())
	{
		DefaultPawnClass = DefaultPawnClassFinder.Class;
	}
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	APFGameState* PFGameState = GetGameState<APFGameState>();
	if (PFGameState)
	{
		PFGameState->InitPlayerLocations();
	}

	// AsyncTask(ENamedThreads::Type::GameThread, [this]()
	// {
	// 	
	// });

	// DEBUG_MESSAGE(TEXT("PlayerController Count [%d]"), GetWorld()->GetNumPlayerControllers());
	
}

AActor* ABattleGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	// fall back
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

void ABattleGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);

	ErrorMessage = TEXT("Battle has Started");
}
