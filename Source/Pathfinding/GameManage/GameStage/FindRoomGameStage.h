#pragma once

#include "GameStage.h"

class PATHFINDING_API FFindRoomGameStage final : public IGameStage
{
public:
	virtual FName GetStageName() const override { return TEXT("FindRoom"); }
	virtual bool CanTransition(UPFGameInstance* GameInstance) override;
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;

private:
	static FName WidgetName;
};
