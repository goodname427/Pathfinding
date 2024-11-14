#pragma once

#include "GameStage.h"

class PATHFINDING_API FFindRoomGameStage final : public IGameStage
{
public:
	virtual FName GetStageName() const override { return TEXT("FindRoomGameStage"); }
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
};
