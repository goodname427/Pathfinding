// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleGameMode.h"

#include "BattleGameState.h"
#include "BattleHUD.h"
#include "BattlePlayerState.h"
#include "Camp.h"
#include "CommanderPawn.h"
#include "EditorCategoryUtils.h"
#include "EngineUtils.h"
#include "NavigationSystem.h"
#include "PFUtils.h"
#include "Building/BaseCampPawn.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

ABattleGameMode::ABattleGameMode()
{
	bUseSeamlessTravel = true;

	GameStateClass = ABattleGameState::StaticClass();
	PlayerStateClass = ABattlePlayerState::StaticClass();
	HUDClass = ABattleHUD::StaticClass();
	DefaultPawnClass = ACommanderPawn::StaticClass();
}

void ABattleGameMode::BeginPlay()
{
	Super::BeginPlay();

	APFGameState* PFGameState = GetGameState<APFGameState>();
	if (PFGameState)
	{
		PFGameState->InitPlayerLocations();
	}

	// DEBUG_MESSAGE(TEXT("Num of Controller: %d"), GetWorld()->GetNumPlayerControllers());
	// TArray<AActor*> FoundActors;
	// UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACommanderPawn::StaticClass(), FoundActors);
	// for (AActor* Actor : FoundActors)
	// {
	// 	DEBUG_MESSAGE(TEXT("Init Player[%s]"), *Actor->GetName());
	// 	if (ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(Actor))
	// 	{
	// 		SpawnDefaultPawnsForCommander(CommanderPawn);
	// 	}
	// }

	// AsyncTask(ENamedThreads::Type::GameThread, [this]()
	// {
	// 		
	// });

	// DEBUG_MESSAGE(TEXT("PlayerController Count [%d]"), GetWorld()->GetNumPlayerControllers());
}

void ABattleGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	Super::FinishRestartPlayer(NewPlayer, StartRotation);

	if (ACommanderPawn* CommanderPawn = Cast<ACommanderPawn>(NewPlayer->GetPawn()))
	{
		// DEBUG_MESSAGE(TEXT("Init Player[%s]"), *NewPlayer->GetName());
		SpawnDefaultPawnsForCommander(CommanderPawn);
	}
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
	const FName PlayerStartTag = FName(FString::Printf(TEXT("p%d"), PlayerState->GetPlayerLocation()));
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

void ABattleGameMode::SpawnDefaultPawnsForCommander(ACommanderPawn* CommanderPawn)
{
	APFPlayerState* PS = CommanderPawn->GetPlayerState<APFPlayerState>();
	const UCamp* Camp = nullptr;
	if (PS)
	{
		Camp = PS->GetCamp();
		if (Camp == nullptr)
		{
			PS->SetCamp(GetDefault<UPFGameSettings>()->GetRandomlyCamp());
		}
	}

	if (!Camp)
	{
		return;
	}
	
	CommanderPawn->SpawnPawn(Camp->GetBaseCampPawnClass(), CommanderPawn->GetActorLocation());

	FVector RandomLocation;
	for (const FDefaultPawnInfo& DefaultPawnInfo : Camp->GetDefaultPawnInfos())
	{
		for (int32 i = 0; i < DefaultPawnInfo.Num; i++)
		{
			if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
				this,
				CommanderPawn->GetActorLocation(),
				RandomLocation,
				1000.0f))
			{
				CommanderPawn->SpawnPawn(DefaultPawnInfo.Class, RandomLocation);
			}
		}
	}
}
