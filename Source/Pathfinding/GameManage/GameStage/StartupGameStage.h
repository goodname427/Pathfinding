#pragma once

#include "GameStage.h"

/**
 * The Default Stage While Game Started, To Determine The Game Just Started.
 */
class PATHFINDING_API FStartupGameStage final : public IGameStage
{
public:
	virtual FName GetStageName() const override { return TEXT("Startup"); }

	virtual void OnWorldBeginPlay(UPFGameInstance* GameInstance, UWorld* World) override;
};
