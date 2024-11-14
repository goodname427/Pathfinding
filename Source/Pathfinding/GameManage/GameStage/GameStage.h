#pragma once

#include "PFGameInstance.h"
#include "MainMenuGameStage.h"
#include "StartupGameStage.h"
#include "RoomGameStage.h"

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

	template<typename... ArgTypes>
	static TSharedPtr<IGameStage> NameToStage(FName InStageName, ArgTypes&&... InArgs);
};

#define RET_STAGE_IF_MATCHED(StageName) \
if (InStageName == #StageName) \
return MakeShared<F##StageName##GameStage>(InArgs...) \

template <typename ... ArgTypes>
TSharedPtr<IGameStage> IGameStage::NameToStage(FName InStageName, ArgTypes&&... InArgs)
{
	RET_STAGE_IF_MATCHED(Startup);
	RET_STAGE_IF_MATCHED(MainMenu);
	RET_STAGE_IF_MATCHED(Room);

	return nullptr;
}
