#pragma once

#include "GameStage.h"
#include "GameFramework/GameSession.h"

/**
 * Player Has Created Or Joined A Session, Waiting For Game Start.  
 */
class PATHFINDING_API FRoomGameStage final : public IGameStage
{
public:
	FRoomGameStage() : FRoomGameStage(nullptr) {};
	FRoomGameStage(AGameSession* InSessionToJoin) : SessionToJoin(InSessionToJoin) {};
	
public:
	virtual FName GetStageName() const override { return "Room"; }
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;

private:
	AGameSession* SessionToJoin;
};
