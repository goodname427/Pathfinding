#include "RoomGameStage.h"

#include "PFGameSession.h"
#include "PFGameInstance.h"
#include "PFPlayerController.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"
#include "MainMenuGameStage.h"
#include "Room/RoomGameState.h"

FName FRoomGameStage::LevelName = FName("L_Room");
FName FRoomGameStage::WidgetName = FName("RoomMenu");

bool FRoomGameStage::CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage)
{
	if (!GameInstance->IsCurrentStage<FMainMenuGameStage>())
	{
		OutErrorMessage = TEXT("Room Stage Can Only Transition From MainMenu Stage");
		return false;	
	}
	
	if ((GameSession = GameInstance->GetGameSession()) == nullptr)
	{
		OutErrorMessage = TEXT("GameSession Is Null");
		return false;
	}

	if (Type == RoomType::None)
	{
		OutErrorMessage = TEXT("Room Type Is None");
		return false;
	}

	return true;
}

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	NULL_CHECK(GameSession);

	// Host Room
	// [Server]
	if (Type == RoomType::Host)
	{
		GameSession->HostRoom(MapToHost.MaxPlayers);
		GameInstance->GetWorld()->ServerTravel(UPFGameInstance::GetURL(LevelName.ToString(), TEXT("?listen")));
	}
	// Join Room
	// [Client]
	else if (Type == RoomType::Join)
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
		GameInstance->GetSubsystem<UWidgetSubsystem>()->PushAndFocus(WidgetName);

		if (World->IsServer())
		{
			ARoomGameState* RoomGameState = World->GetGameState<ARoomGameState>();
			if (RoomGameState)
			{
				RoomGameState->SetCurrentMap(MapToHost);
			}
		}
	}
}
