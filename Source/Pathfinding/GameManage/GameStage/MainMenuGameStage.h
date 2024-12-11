#pragma once

#include "GameStage.h"

/**
 * Main Menu.
 * Player Can Choose The Follow Options:
 * 1. Host Game (Create Room To Play Game)
 * 2. Find Games (Find Other's Game To Play)
 * 3. Quit (Quit Game)
 */
class PATHFINDING_API FMainMenuGameStage final : public IGameStage
{
public:
	virtual FName GetStageName() const override { return TEXT("MainMenu"); }
	virtual void OnEnterStage(UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(UPFGameInstance* GameInstance) override;
	virtual void OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World) override;

private:
	static FName LevelName;
	static FName WidgetName;
};
