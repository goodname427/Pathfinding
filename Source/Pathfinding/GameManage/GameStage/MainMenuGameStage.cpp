#include "MainMenuGameStage.h"

#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "WidgetSubsystem.h"
#include "Kismet/GameplayStatics.h"

FName FMainMenuGameStage::LevelName = FName("L_MainMenu");
FName FMainMenuGameStage::WidgetName = FName("MainMenu");

void FMainMenuGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	// UE_LOG_TEMP(TEXT("Current Level Name: %s"), *GameInstance->GetWorld()->GetName());
	// Disconnect
	if (!GameInstance->GetWorld()->IsNetMode(NM_Standalone))
	{
		Online::GetSubsystem(GameInstance->GetWorld())->GetSessionInterface()->DestroySession(NAME_GameSession);
		// [Server]
		// APFGameSession* GameSession = GameInstance->GetGameSession();
		// if (GameSession)
		// {
		// 	auto PlayerIter = GameInstance->GetWorld()->GetPlayerControllerIterator();
		// 	if (PlayerIter)
		// 	{
		// 		++PlayerIter; // Skip First Player
		// 		for (; PlayerIter; ++PlayerIter)
		// 		{
		// 			GameSession->KickPlayer(PlayerIter->Get(), FText::FromString(TEXT("Room Dismissed")));
		// 		}
		// 	}
		// }
		// [Client]
		// else
		// {
		// 	Online::GetSubsystem(GameInstance->GetWorld())->GetSessionInterface()->DestroySession(NAME_GameSession);
		// }
	}

	if (GameInstance->GetWorld()->GetFName() != LevelName)
	{
		APlayerController* PC = GameInstance->GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->ClientTravel(GameInstance->GetURL(LevelName.ToString()), TRAVEL_Absolute);
		}
	}
	// else
	// {
	// 	GameInstance->GetSubsystem<UWidgetSubsystem>()->PushAndFocus(WidgetName);
	// }
}

void FMainMenuGameStage::OnExitStage(class UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FMainMenuGameStage::OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World)
{
	if (World != nullptr && World->GetFName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->PushAndFocus(WidgetName);
	}
}
