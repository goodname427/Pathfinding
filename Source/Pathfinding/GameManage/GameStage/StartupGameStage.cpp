#include "StartupGameStage.h"

#include "MainMenuGameStage.h"

void FStartupGameStage::OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World)
{
	GameInstance->TransitionToStage<FMainMenuGameStage>();
}
