#pragma once

#include "GameStage.h"
#include "GameFramework/GameSession.h"

/**
 * Player Has Created Or Joined A Session, Waiting For Game Start.  
 */
class PATHFINDING_API FRoomGameStage final : public IGameStage
{
public:
	FRoomGameStage() : FRoomGameStage(-1) {}
	explicit  FRoomGameStage(const int32 InRoomIndexToJoin) : GameSession(nullptr), RoomIndexToJoin(InRoomIndexToJoin) {};
	
	virtual FName GetStageName() const override { return "Room"; }
	virtual bool CanTransition(UPFGameInstance* GameInstance) override;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
	virtual void OnWorldBeginPlay(class UPFGameInstance* GameInstance, UWorld* World) override;
	
private:
	APFGameSession* GameSession;
	
	int32 RoomIndexToJoin;

private:
	static FName LevelName;
	static FName WidgetName;
};
