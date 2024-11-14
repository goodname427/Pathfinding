#pragma once

#include "PFGameInstance.h"

class FRoomGameStage;
class FMainMenuGameStage;
class FStartupGameStage;

/**
 *
 */
class PATHFINDING_API IGameStage
{
protected:
	IGameStage() {}
	virtual ~IGameStage() {}

public:
	virtual FName GetStageName() const = 0;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) {};
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) {};

public:
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2);

	template<typename TGameStage>
	static bool IsSameStage(const TSharedPtr<IGameStage> Stage) { return dynamic_cast<TGameStage>(Stage.Get()); }

	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage, FName StageName);
};


