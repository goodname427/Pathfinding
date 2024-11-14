// Fill out your copyright notice in the Description page of Project Settings.


#include "Online/OnlineHelperSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "WidgetSubsystem.h"
#include "PFUtils.h"
#include "GameFramework/GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/OnlineSession.h"
#include "GameStage/RoomGameStage.h"
#include "Interfaces/OnlineSessionInterface.h"


void UOnlineHelperSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UOnlineHelperSubsystem::DestroySession()
{
	// todo
	// APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
}

void UOnlineHelperSubsystem::HostRoom()
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	FOnlineSessionSettings Settings;
	{
		Settings.bIsLANMatch = true;
	}

	OnlineSubsystem->GetSessionInterface()->CreateSession(
		4,
		FName(GetWorld()->GetFirstPlayerController()->GetLocalPlayer()->GetNickname()),
		Settings
	);
}

void UOnlineHelperSubsystem::JoinRoom()
{
	// todo
	UE_LOG_TEMP(TEXT("Join Room ____"));
}

void UOnlineHelperSubsystem::FindRooms()
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(GetWorld());
	NULL_CHECK(OnlineSubsystem);

	TSharedRef<FOnlineSessionSearch> Settings = MakeShared<FOnlineSessionSearch>();
	{
		Settings->bIsLanQuery = true;
		Settings->MaxSearchResults = 20;
	}
	

	TSharedPtr<FDelegateHandle> DelegateHandle = MakeShared<FDelegateHandle>();
	*DelegateHandle = OnlineSubsystem->GetSessionInterface()->OnFindSessionsCompleteDelegates.AddLambda([Settings](bool bWasSuccess)
	{
		if (bWasSuccess)
		{
			UE_LOG_TEMP(TEXT("Find %d Room"), Settings->SearchResults.Num());

			for (FOnlineSessionSearchResult Result : Settings->SearchResults)
			{
				UE_LOG_TEMP(TEXT("Session Owning User Name: %s"), *Result.Session.OwningUserName);
			}
		}
		else
		{
			UE_LOG_TEMP(TEXT("Find Room Failure")); 
		}
	});
	
	OnlineSubsystem->GetSessionInterface()->FindSessions(0, Settings);
}
