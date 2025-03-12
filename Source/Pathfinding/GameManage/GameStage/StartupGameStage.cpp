#include "StartupGameStage.h"

#include "MainMenuGameStage.h"
#include "PFGameSettings.h"

void FStartupGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	if (const UPFGameSettings* Settings = GetDefault<UPFGameSettings>())
	{
		if (Settings->bTransitionToMainMenuAtBeginning)
		{
			GameInstance->TransitionToStage<FMainMenuGameStage>();
		}
	}
}
