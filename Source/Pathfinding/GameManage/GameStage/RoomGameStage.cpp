#include "RoomGameStage.h"

#include "PFGameSession.h"
#include "PFGameInstance.h"
#include "PFPlayerController.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

FName FRoomGameStage::LevelName = FName("L_Room");
FName FRoomGameStage::WidgetName = FName("RoomMenu");

bool FRoomGameStage::CanTransition(UPFGameInstance* GameInstance)
{
	if ((GameSession = GameInstance->GetGameSession()) == nullptr)
	{
		return false;
	}
	
	return true;
}

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	NULL_CHECK(GameSession);

	// Host Room
	// [Server]
	if (RoomIndexToJoin == INDEX_NONE)
	{
		GameSession->HostRoom();
		GameInstance->GetWorld()->ServerTravel(GameInstance->GetURL( LevelName.ToString(), TEXT("?listen")));
	}
	// Join Room
	// [Client]
	else
	{
		GameSession->JoinRoom(RoomIndexToJoin);
	}
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FRoomGameStage::OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World)
{
	if (World != nullptr && World->GetFName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->ShowAndFocus(WidgetName);
	}
}
