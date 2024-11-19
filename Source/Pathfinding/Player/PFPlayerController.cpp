// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerController.h"
#include "GameStage/RoomGameStage.h"
#include "GameStage/MainMenuGameStage.h"
#include "GameStage/FindRoomGameStage.h"
#include "PFGameInstance.h"
#include "WidgetSubsystem.h"
#include "GameStage/PlayingGameStage.h"

void APFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// SetInputMode(FInputModeGameAndUI().SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways));
	// SetInputMode(FInputModeGameAndUI());
	// SetShowMouseCursor(true);
}

void APFPlayerController::AllTransitionToMainMenuStage()
{
	IMPLEMENT_ALL_TRANSITION_TO_STAGE(MainMenu);
}

void APFPlayerController::TransitionToMainMenuStage_Implementation()
{
	IMPLEMENT_TRANSITION_TO_STAGE(MainMenu);
}

void APFPlayerController::AllTransitionToFindRoomStage()
{
	IMPLEMENT_ALL_TRANSITION_TO_STAGE(FindRoom);
}

void APFPlayerController::TransitionToFindRoomStage_Implementation()
{
	IMPLEMENT_TRANSITION_TO_STAGE(FindRoom);
}

void APFPlayerController::AllTransitionToRoomStage(int32 InRoomIndexToJoin)
{
	IMPLEMENT_ALL_TRANSITION_TO_STAGE(Room, InRoomIndexToJoin);
}

void APFPlayerController::TransitionToRoomStage_Implementation(int32 InRoomIndexToJoin)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Room, InRoomIndexToJoin);
}

void APFPlayerController::AllTransitionToPlayingStage(const FString& InLevelPathToPlay)
{
	IMPLEMENT_ALL_TRANSITION_TO_STAGE(Playing, InLevelPathToPlay);
}

void APFPlayerController::TransitionToPlayingStage_Implementation(const FString& InLevelPathToPlay)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Playing, InLevelPathToPlay);
}
