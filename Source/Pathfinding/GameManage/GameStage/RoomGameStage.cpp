#include "RoomGameStage.h"

#include "LocalizationDescriptor.h"
#include "OnlineHelperSubsystem.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	UOnlineHelperSubsystem* OnlineHelperSubsystem = GameInstance->GetSubsystem<UOnlineHelperSubsystem>();
	NULL_CHECK(OnlineHelperSubsystem);

	// Host Room
	if (SessionToJoin == nullptr)
	{
		OnlineHelperSubsystem->HostRoom();
		GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_Room"), TEXT("Room"), TEXT("listen"));
	}
	// Join Room
	else
	{
		// OnlineHelperSubsystem->JoinRoom(TEXT("L_Room"), TEXT("listen"));
	}
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	
}
