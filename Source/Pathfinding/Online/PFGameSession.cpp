// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/PFGameSession.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "PFGameInstance.h"
#include "PFUtils.h"
#include "GameFramework/GameSession.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"


FSessionSearchResult::FSessionSearchResult(): PingInMS(0), NumCurrentPlayer(0), NumMaxPlayer(0)
{
}

FSessionSearchResult::FSessionSearchResult(const FOnlineSessionSearchResult& InResult)
	: SessionName(InResult.Session.OwningUserName),
	  PingInMS(InResult.PingInMs)
{
	NumMaxPlayer = InResult.Session.SessionSettings.NumPublicConnections + InResult.Session.SessionSettings.
		NumPrivateConnections;
	NumCurrentPlayer = NumMaxPlayer - (InResult.Session.NumOpenPrivateConnections + InResult.Session.
		NumOpenPublicConnections);
}

FUniqueNetIdPtr APFGameSession::GetPlayerUniqueNetId() const
{
	return GetGameInstance()->GetFirstGamePlayer()->GetPreferredUniqueNetId().GetUniqueNetId();
}

UPFGameInstance* APFGameSession::GetPFGameInstance() const
{
	return Cast<UPFGameInstance>(GetGameInstance());
}

void APFGameSession::BeginPlay()
{
	Super::BeginPlay();

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	SessionInterface = OnlineSubsystem->GetSessionInterface();
	GAME_SESSION_CHECK();
	
	BIND_SESSION_ACTION_COMPLETE_DELEGATE(CreateSession);
	BIND_SESSION_ACTION_COMPLETE_DELEGATE(FindSessions);
	BIND_SESSION_ACTION_COMPLETE_DELEGATE(JoinSession);
	BIND_SESSION_ACTION_COMPLETE_DELEGATE(DestroySession);
}

void APFGameSession::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GAME_SESSION_CHECK();

	UNBIND_SESSION_ACTION_COMPLETE_DELEGATE(CreateSession);
	UNBIND_SESSION_ACTION_COMPLETE_DELEGATE(FindSessions);
	UNBIND_SESSION_ACTION_COMPLETE_DELEGATE(JoinSession);
	UNBIND_SESSION_ACTION_COMPLETE_DELEGATE(DestroySession);
}

void APFGameSession::OnCreateSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG_TEMP(TEXT("Create Room [ %s ] Successful"), *SessionName.ToString());
	}
	else
	{
		GetPFGameInstance()->Error(TEXT("Create Room Failure"));
	}
}

void APFGameSession::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG_TEMP(TEXT("Find %d Rooms"), SearchSettings->SearchResults.Num());

		TArray<FSessionSearchResult> SearchResults;
		for (const FOnlineSessionSearchResult& Result : SearchSettings->SearchResults)
		{
			UE_LOG_TEMP(TEXT("Session Owning User Name: %s"),
						*Result.Session.OwningUserName);
			SearchResults.Add(Result);
		}
		if (OnFindRooms.IsBound())
		{
			OnFindRooms.Broadcast(SearchResults);
		}
	}
	else
	{
		GetPFGameInstance()->Error(TEXT("Find Room Failure"));
	}
}

void APFGameSession::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type CompleteResult)
{
	if (CompleteResult == EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG_TEMP(TEXT("Join Room [ %s ] Successful"), *InSessionName.ToString());

		TravelToRoom();
	}
	else
	{
		GetPFGameInstance()->Error(TEXT("Join Room Failure"));
	}
}

void APFGameSession::OnDestroySessionComplete(FName InSessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG_TEMP(TEXT("Dismiss Room [ %s ] Successful"),
					*SessionName.ToString());
	}
	else
	{
		GetPFGameInstance()->Error(TEXT("Dismiss Room Failure"));
	}
}

void APFGameSession::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	GetPFGameInstance()->PostLogin(NewPlayer);
}

void APFGameSession::DismissRoom()
{
	// APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	GAME_SESSION_CHECK();
	
	SessionInterface.Pin()->DestroySession(NAME_GameSession);
}

void APFGameSession::HostRoom(int RoomMaxPlayers)
{
	GAME_SESSION_CHECK();

	SessionSettings = MakeShared<FOnlineSessionSettings>();
	{
		SessionSettings->bIsLANMatch = true;
		SessionSettings->NumPublicConnections = RoomMaxPlayers;
		SessionSettings->NumPrivateConnections = 0;
		SessionSettings->bShouldAdvertise = true;
	}
	
	SessionInterface.Pin()->CreateSession(
		*GetPlayerUniqueNetId(),
		// FName(FString::Printf(TEXT("%s's Room"), *GetGameInstance()->GetFirstGamePlayer()->GetNickname())),
		NAME_GameSession,
		*SessionSettings
	);
}

void APFGameSession::JoinRoom(const int32 RoomIndex)
{
	GAME_SESSION_CHECK();
	
	if (RoomIndex < 0 || RoomIndex >= SearchSettings->SearchResults.Num())
	{
		return;
	}
	
	SessionInterface.Pin()->JoinSession(*GetPlayerUniqueNetId(), NAME_GameSession,
	                              SearchSettings->SearchResults[RoomIndex]);
}

void APFGameSession::FindRooms()
{
	GAME_SESSION_CHECK();

	SearchSettings = MakeShared<FOnlineSessionSearch>();
	{
		SearchSettings->bIsLanQuery = true;
		SearchSettings->MaxSearchResults = 20;
	}

	SessionInterface.Pin()->FindSessions(
		*GetPlayerUniqueNetId(),
		SearchSettings
	);
}

void APFGameSession::TravelToRoom()
{
	GAME_SESSION_CHECK();

	FString URL;
	if (SessionInterface.Pin()->GetResolvedConnectString(SessionName, URL))
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->ClientTravel(URL, TRAVEL_Absolute);
		}
	}
}

void APFGameSession::ChangeMaxPlayer(int32 InMaxPlayer)
{
	GAME_SESSION_CHECK();
}