#pragma once

#include "PFGameInstance.h"

/**
 *
 */
class PATHFINDING_API IGameStage
{
protected:
	IGameStage() {}
	virtual ~IGameStage() {}

public:
	virtual FString GetStageName() const = 0;
	virtual void OnExitStage(class UPFGameInstance* GameInstance) {};
	virtual void OnEnterStage(class UPFGameInstance* GameInstance) {};

public:
	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage1, const TSharedPtr<IGameStage>& Stage2);

	template<typename TGameStage>
	static bool IsSameStage(const TSharedPtr<IGameStage> Stage) { return dynamic_cast<TGameStage>(Stage.Get()); }

	static bool IsSameStage(const TSharedPtr<IGameStage>& Stage, const FString& StageName);

	static TSharedPtr<IGameStage> NameToStage(const FString& InStageName);
};
