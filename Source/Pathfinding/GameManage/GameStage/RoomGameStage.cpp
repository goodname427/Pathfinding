#include "RoomGameStage.h"

#include "PFGameSession.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	APFGameSession* GameSession = GameInstance->GetGameSession();
	NULL_CHECK(GameSession);

	// Host Room
	if (RoomIndexToJoin == INDEX_NONE)
	{
		GameSession->HostRoom();
		GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_Room"), TEXT("Room"), TEXT("listen"));
	}
	// Join Room
	else
	{
		GameSession->JoinRoom(RoomIndexToJoin);
		GameSession->TravelToRoom();
	}
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	
}
