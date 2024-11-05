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
	FMainMenuGameStage() {}

	virtual FString GetStageName() const override { return TEXT("MainMenu"); }
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) override;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) override;
};
