#pragma once

#include "GameStage.h"
#include "PFGameSettings.h"
#include "GameFramework/GameSession.h"

enum class RoomType
{
	None,
	Host,
	Join
};

/**
 * Player Has Created Or Joined A Session, Waiting For Game Start.  
 */
class PATHFINDING_API FRoomGameStage final : public IGameStage
{
public:
	FRoomGameStage() : GameSession(nullptr), Type(RoomType::None),
	                   RoomIndexToJoin(-1), MapToHost()
	{
	}

	explicit FRoomGameStage(const int32 InRoomIndexToJoin) : GameSession(nullptr), Type(RoomType::Join),
	                                                         RoomIndexToJoin(InRoomIndexToJoin), MapToHost()
	{
	};

	explicit FRoomGameStage(const FMapInfo& InMapInfo) : GameSession(nullptr), Type(RoomType::Host),
	                                                     RoomIndexToJoin(-1), MapToHost(InMapInfo)
	{
	};

	virtual FName GetStageName() const override { return "Room"; }
	virtual bool CanTransition(UPFGameInstance* GameInstance, FString& OutErrorMessage) override;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
	virtual void OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World) override;

private:
	APFGameSession* GameSession;

	RoomType Type;
	int32 RoomIndexToJoin;
	FMapInfo MapToHost;

private:
	static FName LevelName;
	static FName WidgetName;
};
