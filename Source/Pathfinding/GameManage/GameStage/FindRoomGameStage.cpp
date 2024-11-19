#include "FindRoomGameStage.h"
#include "MainMenuGameStage.h"
#include "WidgetSubsystem.h"

FName FFindRoomGameStage::WidgetName = FName("ServerList");

bool FFindRoomGameStage::CanTransition(UPFGameInstance* GameInstance)
{
	if (!GameInstance->IsCurrentStage<FMainMenuGameStage>())
	{
		return false;
	}
	return true;
}

void FFindRoomGameStage::OnEnterStage(class UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->ShowAndFocus(WidgetName);
}
