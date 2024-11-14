#include "FindRoomGameStage.h"

#include "WidgetSubsystem.h"

void FFindRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	if (!GameInstance->IsCurrentStage(TEXT("MainMenu")))
	{
		return;
	}
	
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Show("ServerList");
}

void FFindRoomGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	IGameStage::OnExitStage(GameInstance);
}
