#include "PlayingGameStage.h"

#include "RoomGameStage.h"
#include "WidgetSubsystem.h"

FName FPlayingGameStage::WidgetName = FName("PlayMenu");

bool FPlayingGameStage::CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage)
{
	if (!GameInstance->IsCurrentStage<FRoomGameStage>())
	{
		OutErrorMessage = TEXT("Playing Stage Can Only Transition From Room Stage");
		return false;
	}
	return true;
}

void FPlayingGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	// [Server]
	if (GameInstance->GetWorld()->IsServer())
	{
		GameInstance->GetWorld()->ServerTravel(GameInstance->GetURL(LevelPathToPlay, TEXT("?listen")));
	}
}

void FPlayingGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FPlayingGameStage::OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World)
{
	FString LevelName = FPaths::GetBaseFilename(LevelPathToPlay);
	if (World != nullptr && World->GetName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);

		APlayerController* PC = GameInstance->GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}
