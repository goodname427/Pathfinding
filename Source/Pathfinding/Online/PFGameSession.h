// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameSession.h"
#include "GameStage/GameStage.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PFGameSession.generated.h"

USTRUCT(BlueprintType)
struct FSessionSearchResult
{
	GENERATED_USTRUCT_BODY()

	FSessionSearchResult();
	FSessionSearchResult(const FOnlineSessionSearchResult& InResult);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SessionName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 PingInMS;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumCurrentPlayer;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumMaxPlayer;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFoundRoomsSignature, const TArray<FSessionSearchResult>&,
                                            SessionSearchResults);

#define DECLARE_SESSION_ACTION_COMPLETE_DELEGATE(ActionName, ...) \
FDelegateHandle On##ActionName##CompleteDelegateHandle; \
void On##ActionName##Complete(__VA_ARGS__);

#define BIND_SESSION_ACTION_COMPLETE_DELEGATE(ActionName) \
On##ActionName##CompleteDelegateHandle = SessionInterface.Pin()->AddOn##ActionName##CompleteDelegate_Handle(FOn##ActionName##CompleteDelegate::CreateUObject(this, &ThisClass::On##ActionName##Complete))

#define UNBIND_SESSION_ACTION_COMPLETE_DELEGATE(ActionName) \
SessionInterface.Pin()->ClearOn##ActionName##CompleteDelegate_Handle(On##ActionName##CompleteDelegateHandle)

#define GAME_SESSION_CHECK() \
	if (!IsReady()) return

/**
 * 
 */
UCLASS()
class PATHFINDING_API APFGameSession : public AGameSession
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FFoundRoomsSignature OnFoundRooms;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	bool IsReady() const { return SessionInterface.IsValid(); }
	
	TWeakPtr<IOnlineSession, ESPMode::ThreadSafe> SessionInterface;
	
private:
	DECLARE_SESSION_ACTION_COMPLETE_DELEGATE(CreateSession, FName InSessionName, bool bWasSuccessful)
	DECLARE_SESSION_ACTION_COMPLETE_DELEGATE(FindSessions, bool bWasSuccessful)
	DECLARE_SESSION_ACTION_COMPLETE_DELEGATE(JoinSession, FName InSessionName, EOnJoinSessionCompleteResult::Type CompleteResult)
	DECLARE_SESSION_ACTION_COMPLETE_DELEGATE(DestroySession, FName InSessionName, bool bWasSuccessful)

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;;
	virtual bool AtCapacity(bool bSpectator) override;

public:
	// Network
	UFUNCTION(BlueprintCallable)
	void DismissRoom();
	
	UFUNCTION(BlueprintCallable)
	void HostRoom(int RoomMaxPlayers = 10);

	UFUNCTION(BlueprintCallable)
	void JoinRoom(int32 RoomIndex);

	UFUNCTION(BlueprintCallable)
	void FindRooms();

	UFUNCTION(BlueprintCallable)
	void TravelToRoom();

	UFUNCTION(BlueprintCallable)
	virtual bool KickPlayer(APlayerController* KickedPlayer, const FText& KickReason) override;

	bool IsRoomFull(int32 RoomIndex) const;
	
protected:
	FUniqueNetIdPtr GetPlayerUniqueNetId() const;

	class UPFGameInstance* GetPFGameInstance() const;

	FNamedOnlineSession* GetCurrentOnlineSession() const;

private:
	TSharedRef<FOnlineSessionSearch> SearchSettings = MakeShareable(new FOnlineSessionSearch());;
	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());;
};
