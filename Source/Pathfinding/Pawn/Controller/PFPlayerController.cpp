// Fill out your copyright notice in the Description page of Project Settings.


#include "PFPlayerController.h"

#include "PFGameInstance.h"
#include "PFUtils.h"
#include "GameStage/RoomGameStage.h"
#include "GameStage/MainMenuGameStage.h"
#include "GameFramework/PlayerState.h"
#include "GameStage/PlayingGameStage.h"
#include "GameStage/SettlementGameStage.h"

APFPlayerController::APFPlayerController()
{
	bShowMouseCursor = true;
}

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

void APFPlayerController::TransitionToRoomStageByJoin_Implementation(int32 InRoomIndexToJoin)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Room, InRoomIndexToJoin);
}

void APFPlayerController::TransitionToRoomStageByHost_Implementation(FMapInfo InMapInfo)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Room, InMapInfo);
}

void APFPlayerController::AllTransitionToPlayingStage(const FString& InLevelPathToPlay)
{
	IMPLEMENT_ALL_TRANSITION_TO_STAGE(Playing, InLevelPathToPlay);
}

void APFPlayerController::TransitionToPlayingStage_Implementation(const FString& InLevelPathToPlay)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Playing, InLevelPathToPlay);
}

void APFPlayerController::AllTransitionToSettlementStage()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		for (auto PCIter = GetWorld()->GetPlayerControllerIterator(); PCIter; ++PCIter)
		{
			// Skip this player
			if (*PCIter == this)
			{
				continue;
			}
			
			if (APFPlayerController* PF = Cast<APFPlayerController>(*PCIter))
			{
				if (const ABattlePlayerState* PS = PF->GetPlayerState<ABattlePlayerState>())
				{
					PF->TransitionToSettlementStage(PS->HasFailed());
				}
			}
		}

		// Self transition to settlement stage on the tail
		Delay(this, 0.1f, [this]
		{
			if (const ABattlePlayerState* PS = GetPlayerState<ABattlePlayerState>())
			{
				TransitionToSettlementStage(PS->HasFailed());
			}
		});
	}
}

void APFPlayerController::TransitionToSettlementStage_Implementation(bool bFailed)
{
	IMPLEMENT_TRANSITION_TO_STAGE(Settlement, bFailed);
}
