#include "StartupGameStage.h"

#include "MainMenuGameStage.h"

void FStartupGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	GameInstance->TransitionToStage<FMainMenuGameStage>();
}
