#pragma once

#include "GameStage.h"
#include "StartupGameStage.h"
#include "RoomGameStage.h"
#include "MainMenuGameStage.h"

#define RET_STAGE_IF_MATCHED(StageName) \
if (InStageName == #StageName) \
	return MakeShared<F##StageName##GameStage>(InArgs...)

class PATHFINDING_API FGameStageHelper
{
public:
	template <typename... ArgTypes>
	static TSharedPtr<IGameStage> NameToStage(FName InStageName, ArgTypes&&... InArgs);
};

template <typename... ArgTypes>
TSharedPtr<IGameStage> FGameStageHelper::NameToStage(FName InStageName, ArgTypes&&... InArgs)
{
	RET_STAGE_IF_MATCHED(Startup);
	RET_STAGE_IF_MATCHED(MainMenu);
	RET_STAGE_IF_MATCHED(Room);
	return nullptr;
}
