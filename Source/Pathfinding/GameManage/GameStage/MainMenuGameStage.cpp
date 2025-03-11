#include "MainMenuGameStage.h"

#include "Online.h"
#include "OnlineSubsystemUtils.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"
#include "StartupGameStage.h"
#include "Kismet/GameplayStatics.h"

FName FMainMenuGameStage::LevelName = FName("L_MainMenu");
FName FMainMenuGameStage::WidgetName = FName("MainMenu");

void FMainMenuGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	const auto SessionInterface = Online::GetSubsystem(GameInstance->GetWorld())->GetSessionInterface();

	APFGameSession* GS = GameInstance->GetGameSession();
	// [Server]
	// The server will destroy the room and then all clients will return to main menu. 
	if (GS)
	{
		GS->DismissRoom();
	}
	// [Client]
	// The client will break the connection and then return to main menu.
	else
	{
		SessionInterface->DestroySession(
			NAME_GameSession,
			FOnDestroySessionCompleteDelegate::CreateLambda(
				[](FName GameSessionName, bool bSuccess)
				{
					UE_LOG_TEMP(TEXT("Destroy Session [%s] %s"),
								*GameSessionName.ToString(),
								bSuccess ? TEXT("Success") : TEXT("Failure"));
				}
			)
		);
	}
	
	if (GameInstance->GetWorld()->GetFName() != LevelName)
	{
		APlayerController* PC = GameInstance->GetFirstLocalPlayerController();
		// GameInstance->GetWorld()->GetFirstPlayerController();
		if (PC)
		{
			PC->ClientTravel(GameInstance->GetURL(LevelName.ToString()), TRAVEL_Absolute);
		}
	}
	else
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);
	}
}

void FMainMenuGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FMainMenuGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	if (World != nullptr && World->GetFName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);
	}
}
