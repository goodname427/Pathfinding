#include "RoomGameStage.h"

#include "LocalizationDescriptor.h"
#include "OnlineHelperSubsystem.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	UOnlineHelperSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UOnlineHelperSubsystem>();
	NULL_CHECK(NetworkSubsystem);

	NetworkSubsystem->HostRoom();
	GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_Room"), TEXT("Room"), TEXT("listen"));
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	
}
