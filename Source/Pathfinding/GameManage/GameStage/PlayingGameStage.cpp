#include "PlayingGameStage.h"

#include "RoomGameStage.h"
#include "WidgetSubsystem.h"

FName FPlayingGameStage::WidgetName = FName("PlayMenu");

bool FPlayingGameStage::CanTransition(UPFGameInstance* GameInstance)
{
	if (!GameInstance->IsCurrentStage<FRoomGameStage>())
	{
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

void FPlayingGameStage::OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World)
{
	FString LevelName = FPaths::GetBaseFilename(LevelPathToPlay);
	if (World != nullptr && World->GetName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Show(WidgetName);

		APlayerController* PC = GameInstance->GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->SetShowMouseCursor(true);
			PC->SetInputMode(FInputModeGameAndUI());
		}
	}
}
