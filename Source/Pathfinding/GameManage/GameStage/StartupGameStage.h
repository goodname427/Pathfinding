#pragma once

#include "GameStage.h"

/**
 * The Default Stage While Game Started, To Determine The Game Just Started.
 */
class PATHFINDING_API FStartupGameStage final : public IGameStage
{
public:
	FStartupGameStage() {}

	virtual FName GetStageName() const override { return TEXT("Startup"); }
};
