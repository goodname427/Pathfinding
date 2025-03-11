#include "SettlementGameStage.h"

#include "OnlineSubsystemUtils.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"
#include "PlayingGameStage.h"
#include "Settlement/SettlementGameState.h"

FName FSettlementGameStage::LevelName = FName("L_Settlement");
FName FSettlementGameStage::WidgetName = FName("SettlementMenu");

bool FSettlementGameStage::CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage)
{
	if (!GameInstance->IsCurrentStage<FPlayingGameStage>())
	{
		OutErrorMessage = TEXT("Settlement Stage Can Only Transition From Playing Stage");
		return false;
	}
	return true;
}

void FSettlementGameStage::OnEnterStage(UPFGameInstance* GameInstance)
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

	APlayerController* PC = GameInstance->GetFirstLocalPlayerController();
	if (PC)
	{
		PC->ClientTravel(GameInstance->GetURL(LevelName.ToString()), TRAVEL_Absolute);
	}
}

void FSettlementGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();
}

void FSettlementGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	if (World != nullptr && World->GetFName() == LevelName)
	{
		ASettlementGameState* SettlementGameState = World->GetGameState<ASettlementGameState>();
		if (SettlementGameState)
		{
			SettlementGameState->bFailed = bFailed;
		}
		
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);
	}
}
