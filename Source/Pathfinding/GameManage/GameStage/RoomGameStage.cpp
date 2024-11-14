#include "RoomGameStage.h"

#include "LocalizationDescriptor.h"
#include "NetworkSubsystem.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"

void FRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	UNetworkSubsystem* NetworkSubsystem = GameInstance->GetSubsystem<UNetworkSubsystem>();
	NULL_CHECK(NetworkSubsystem);

	NetworkSubsystem->HostRoom();
	GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_Room"), TEXT("Room"), TEXT("listen"));
}

void FRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	
}
