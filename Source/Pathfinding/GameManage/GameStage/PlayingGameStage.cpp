#include "PlayingGameStage.h"

#include "PFUtils.h"
#include "RoomGameStage.h"
#include "WidgetSubsystem.h"

FName FPlayingGameStage::WidgetName = FName("PlayingMenu");

bool FPlayingGameStage::CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage)
{
	if (!GameInstance->IsCurrentStage<FRoomGameStage>())
	{
		OutErrorMessage = TEXT("Playing Stage Can Only Transition From Room Stage");
		return false;
	}
	return true;
}

void FPlayingGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	// [Server]
	if (GameInstance->GetWorld()->IsServer())
	{
		GameInstance->GetWorld()->ServerTravel(UPFGameInstance::GetURL(LevelPathToPlay, TEXT("?listen")));
		APFGameSession* GameSession = GameInstance->GetGameSession();
		if (GameSession)
		{
			GameSession->StartRoom();
		}
	}
}

void FPlayingGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FPlayingGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	FString LevelName = FPaths::GetBaseFilename(LevelPathToPlay);
	if (World != nullptr && World->GetName() == LevelName)
	{
		// GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);

		// if (World->IsServer())
		// {
		// }

		// for (auto PCIter = World->GetPlayerControllerIterator(); PCIter; ++PCIter)
		// {
		// 	(*PCIter)->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		// }
			
		APlayerController* PC = World->GetFirstLocalPlayerFromController()->GetPlayerController(World);
		if (PC != nullptr)
		{
			DEBUG_MESSAGE(TEXT("Set PlayerController [%s]"), *PC->GetName());
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI().SetHideCursorDuringCapture(false));
		}
	}
}
