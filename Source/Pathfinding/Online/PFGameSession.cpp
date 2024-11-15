// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/PFGameSession.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "PFUtils.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"


FSessionSearchResult::FSessionSearchResult(): PingInMS(0), NumMaxPlayer(0), NumCurrentPlayer(0)
{
}

FSessionSearchResult::FSessionSearchResult(const FOnlineSessionSearchResult& InResult)
	: SessionName(InResult.Session.GetSessionIdStr()),
	  PingInMS(InResult.PingInMs)
{
	NumMaxPlayer = InResult.Session.SessionSettings.NumPublicConnections + InResult.Session.SessionSettings.NumPrivateConnections;
	NumCurrentPlayer = NumMaxPlayer - (InResult.Session.NumOpenPrivateConnections + InResult.Session.NumOpenPublicConnections);
}

FUniqueNetIdPtr APFGameSession::GetPlayerUniqueNetId() const
{
	return GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
}

void APFGameSession::DestroySession()
{
	// todo
	// APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
}

void APFGameSession::HostRoom()
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	FOnlineSessionSettings Settings;
	{
		Settings.bIsLANMatch = true;
		Settings.NumPublicConnections = 10;
		Settings.NumPrivateConnections = 0;
		Settings.bShouldAdvertise = true;
	}

	IOnlineSessionPtr Sessions = OnlineSubsystem->GetSessionInterface();

	if (Sessions.IsValid())
	{
		// TSharedPtr<FDelegateHandle> DelegateHandle = MakeShared<FDelegateHandle>();
		// *DelegateHandle = Sessions->OnCreateSessionCompleteDelegates.AddLambda(
		// 	[DelegateHandle, Sessions](FName GameSessionName, bool bWasSuccessful)
		// 	{
		// 		if (bWasSuccessful)
		// 		{
		// 			UE_LOG_TEMP(TEXT("Create Room [ %s ] Successful"), *GameSessionName.ToString());
		// 		}
		// 		else
		// 		{
		// 			UE_LOG_TEMP(TEXT("Create Room Failure"));
		// 		}
		//
		// 		Sessions->OnFindSessionsCompleteDelegates.Remove(*DelegateHandle);
		// 	});

		Sessions->CreateSession(
			*GetPlayerUniqueNetId(),
			// FName(FString::Printf(TEXT("%s's Room"), *GetGameInstance()->GetFirstGamePlayer()->GetNickname())),
			NAME_GameSession,
			Settings
		);
	}
}

void APFGameSession::JoinRoom(const int32 RoomIndex)
{
	if (RoomIndex < 0 || RoomIndex >= SearchSettings->SearchResults.Num())
	{
		return;
	}

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		TSharedPtr<FDelegateHandle> DelegateHandle = MakeShared<FDelegateHandle>();
		*DelegateHandle = SessionInterface->OnJoinSessionCompleteDelegates.AddLambda(
			[DelegateHandle, SessionInterface](FName GameSessionName, EOnJoinSessionCompleteResult::Type CompleteResult)
			{
				if (CompleteResult == EOnJoinSessionCompleteResult::Success)
				{
					UE_LOG_TEMP(TEXT("Join Room [ %s ] Successful"), *GameSessionName.ToString());
				}
				else
				{
					UE_LOG_TEMP(TEXT("Join Room Failure"));
				}
		
				SessionInterface->OnJoinSessionCompleteDelegates.Remove(*DelegateHandle);
			});
		
		SessionInterface->JoinSession(*GetPlayerUniqueNetId(), NAME_GameSession, SearchSettings->SearchResults[RoomIndex]);
	}
}

void APFGameSession::FindRooms()
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	SearchSettings = MakeShared<FOnlineSessionSearch>();
	{
		SearchSettings->bIsLanQuery = true;
		SearchSettings->MaxSearchResults = 20;
	}

	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid())
	{
		TSharedPtr<FDelegateHandle> DelegateHandle = MakeShared<FDelegateHandle>();
		*DelegateHandle = SessionInterface->OnFindSessionsCompleteDelegates.AddLambda(
			[DelegateHandle, SessionInterface, this](bool bWasSuccessful)
			{
				if (bWasSuccessful)
				{
					// UE_LOG_TEMP(TEXT("Find %d Rooms"), SearchSettings->SearchResults.Num());

					TArray<FSessionSearchResult> SearchResults;
					for (const FOnlineSessionSearchResult& Result : SearchSettings->SearchResults)
					{
						// UE_LOG_TEMP(TEXT("Session Owning User Name: %s"),
						//             *Result.GetSessionIdStr());
						SearchResults.Add(Result);
					}

					OnFindRooms.Broadcast(SearchResults);
				}
				// else
				// {
				// 	UE_LOG_TEMP(TEXT("Find Room Failure"));
				// }

				SessionInterface->OnFindSessionsCompleteDelegates.Remove(*DelegateHandle);
			});

		SessionInterface->FindSessions(
			*GetPlayerUniqueNetId(),
			SearchSettings
		);
	}
}

void APFGameSession::TravelToRoom()
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);
	
	FString URL;
	IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
	if (SessionInterface.IsValid() && SessionInterface->GetResolvedConnectString(SessionName, URL))
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->ClientTravel(URL, TRAVEL_Absolute);
		}
	}

}
