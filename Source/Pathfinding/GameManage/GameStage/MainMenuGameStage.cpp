#include "MainMenuGameStage.h"
#include "WidgetSubsystem.h"
#include "Kismet/GameplayStatics.h"

void FMainMenuGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	// UE_LOG_TEMP(TEXT("Current Level Name: %s"), *GameInstance->GetWorld()->GetName());

	if (GameInstance->GetWorld()->GetName() != TEXT("L_MainMenu"))
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->OpenLevelAndShow(TEXT("L_MainMenu"), TEXT("MainMenu"));
		return;
	}

	GameInstance->GetSubsystem<UWidgetSubsystem>()->Show(TEXT("MainMenu"));
}

void FMainMenuGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
}
