#include "RoomGameStage.h"

#include "PFGameSession.h"
#include "PFGameInstance.h"
#include "Controller/PFPlayerController.h"
#include "PFUtils.h"
#include "WidgetSubsystem.h"
#include "MainMenuGameStage.h"
#include "Room/RoomGameState.h"

FName FRoomGameStage::LevelName = FName("L_Room");
FName FRoomGameStage::WidgetName = FName("RoomMenu");

bool FRoomGameStage::CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage)
{
	if (!GameInstance->IsCurrentStage<FMainMenuGameStage>())
	{
		OutErrorMessage = TEXT("Room Stage Can Only Transition From MainMenu Stage");
		return false;
	}

	if ((GameSession = GameInstance->GetGameSession()) == nullptr)
	{
		OutErrorMessage = TEXT("GameSession Is Null");
		return false;
	}
	// else if (Type == RoomType::Join && GameSession->IsRoomFull(RoomIndexToJoin))
	// {
	// 	OutErrorMessage = TEXT("Room is Full");
	// 	return false;
	// }

	if (Type == RoomType::None)
	{
		OutErrorMessage = TEXT("Room Type Is None");
		return false;
	}


	return true;
}

void FRoomGameStage::OnEnterStage(UPFGameInstance* GameInstance)
{
	NULL_CHECK(GameSession);

	// Host Room
	// [Server]
	if (Type == RoomType::Host)
	{
		GameSession->HostRoom(MapToHost.MaxPlayers, FOnCreateSessionCompleteDelegate::CreateLambda(
			                      [GameInstance](FName InSessionName, bool bWasSuccessful)
			                      {
				                      if (bWasSuccessful)
				                      {
					                      GameInstance->GetWorld()->ServerTravel(
						                      UPFGameInstance::GetURL(TEXT("L_Room"), TEXT("?listen")));
				                      }
				                      else
				                      {
					                      GameInstance->TransitionToStage<FMainMenuGameStage>();
				                      }
			                      }));
		// GameInstance->GetWorld()->ServerTravel(UPFGameInstance::GetURL(LevelName.ToString(), TEXT("?listen")));
	}
	// Join Room
	// [Client]
	else if (Type == RoomType::Join)
	{
		// GameInstance->ShowLoadingScreen(TEXT("Join Room"));
		GameSession->JoinRoom(RoomIndexToJoin, FOnJoinSessionCompleteDelegate::CreateLambda(
			                      [GameInstance, this](FName InSessionName, EOnJoinSessionCompleteResult::Type InResult)
			                      {
				                      if (InResult == EOnJoinSessionCompleteResult::Success)
				                      {
					                      GameSession->TravelToRoom();
				                      }
				                      else
				                      {
					                      GameInstance->TransitionToStage<FMainMenuGameStage>();
				                      }
			                      }));
	}
}

void FRoomGameStage::OnExitStage(UPFGameInstance* GameInstance)
{
	GameInstance->GetSubsystem<UWidgetSubsystem>()->Clear();

	if (UWorld* World = GameInstance->GetWorld())
	{
		if (World->IsServer())
		{
			const UPFGameSettings* Settings = GetDefault<UPFGameSettings>();
			for (auto PCIter = World->GetPlayerControllerIterator(); PCIter; ++PCIter)
			{
				if (APFPlayerState* PS = PCIter->Get()->GetPlayerState<APFPlayerState>())
				{
					if (PS->GetCampInfo() == nullptr)
					{
						PS->SetCampInfo(Settings->GetRandomlyCamp());
					}
				}
			}
		}
	}
}

void FRoomGameStage::OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World)
{
	if (World != nullptr && World->GetFName() == LevelName)
	{
		GameInstance->GetSubsystem<UWidgetSubsystem>()->Push(WidgetName);

		if (World->IsServer())
		{
			ARoomGameState* RoomGameState = World->GetGameState<ARoomGameState>();
			if (RoomGameState)
			{
				RoomGameState->SetCurrentMap(MapToHost);
			}
		}
	}
}
