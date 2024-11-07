#pragma once

#include "GameStage.h"

/**
 * Player Has Created Or Joined A Session, Waiting For Game Start.  
 */
class PATHFINDING_API FRoomGameStage final : public IGameStage
{
public:
	virtual FName GetStageName() const override { return "Room"; }
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
};
