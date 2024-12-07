#include "PlayingGameStage.h"

#include "PFUtils.h"
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
		DEBUG_MESSAGE(TEXT("%s"), *LevelPathToPlay);
		GameInstance->GetWorld()->ServerTravel(UPFGameInstance::GetURL(LevelPathToPlay, TEXT("?listen")));
		APFGameSession* GameSession = GameInstance->GetGameSession();
		if (GameSession)
		{
			GameSession->StartRoom();
		}
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
	}
}
