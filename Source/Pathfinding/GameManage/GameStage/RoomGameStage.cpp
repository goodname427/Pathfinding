#include "RoomGameStage.h"

#include "OnlineHelperSubsystem.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	UOnlineHelperSubsystem* OnlineHelperSubsystem = GameInstance->GetSubsystem<UOnlineHelperSubsystem>();
	NULL_CHECK(OnlineHelperSubsystem);

	// Host Room
	if (RoomIndexToJoin == INDEX_NONE)
	{
		OnlineHelperSubsystem->HostRoom();
		GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_Room"), TEXT("Room"), TEXT("listen"));
	}
	// Join Room
	else
	{
		OnlineHelperSubsystem->JoinRoom(RoomIndexToJoin);
		OnlineHelperSubsystem->TravelToRoom();
	}
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	
}
